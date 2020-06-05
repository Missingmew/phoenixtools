#include <stdlib.h>

#include "ir.h"

struct lut {
	unsigned numfixup;
	unsigned **fixup;
	struct ir_special data;
};

int lutcompare(const void *a, const void *b) {
	struct lut *left = (struct lut *)a;
	struct lut *right = (struct lut *)b;
	int sec, off;
	sec = left->data.section - right->data.section;
	off = left->data.offset - right->data.offset;
	if(sec != 0) return sec;
	return off;
}

int findidx(struct lut *lut, unsigned valid, uint16_t offset, uint16_t section) {
	for(unsigned i = 0; i < valid; i++) {
		if(lut[i].data.offset == offset && lut[i].data.section == section) return i;
	}
	return -1;
}

void ir_script_fixup(struct ir_script *script, unsigned gamenum) {
	uint32_t curoffset;
	uint16_t targetsec, targetoff;
	unsigned cursec, curcmd, curdata, curspecial, tofree;
	int idx;
	struct ir_section *section;
	struct ir_generic *command;
	struct lut *LUT;
	if(script->numspecials) {
		LUT = malloc(sizeof(struct lut) * script->numspecials);
		for(unsigned i = 0; i < script->numspecials; i++) {
			LUT[i].numfixup = 0;
			LUT[i].fixup = malloc(sizeof(unsigned *) * script->numspecials);
		}
		tofree = script->numspecials;
		script->specials = malloc(sizeof(struct ir_special) * script->numspecials);
	
		/* find and set up specials area */
		curspecial = 0;
		for(cursec = 0; cursec < script->numsections; cursec++) {
			section = script->secarr[cursec];
			for(curcmd = 0; curcmd < section->numcommands; curcmd++) {
				command = section->commands[curcmd];
				switch(command->type) {
					case CMD35:
					case CMD36: 
					case CMD78: {
						for(curdata = 0; curdata < command->numdata; curdata++) {
							if(command->data[curdata].type == DATASECOFF) {
								targetoff = command->data[curdata].data & 0xFFFF;
								targetsec = (command->data[curdata].data >> 16) & 0xFFFF;
								/* check if we have a usable LUT entry on hand, if so use that */
								if((idx = findidx(LUT, curspecial, targetoff, targetsec)) > -1) {
									//~ command->data[curdata].data = script->numsections+idx;
									
									LUT[idx].fixup[LUT[idx].numfixup++] = &(command->data[curdata].data);
								}
								/* otherwise create a new lut entry */
								else {
									//~ script->specials[curspecial].offset = targetoff;
									//~ script->specials[curspecial].section = targetsec;
									//~ command->data[curdata].data = script->numsections+curspecial;
									
									LUT[curspecial].data.offset = targetoff;
									LUT[curspecial].data.section = targetsec;
									LUT[curspecial].fixup[LUT[curspecial].numfixup++] = &(command->data[curdata].data);
									
									curspecial++;
								}
								
								command->data[curdata].type = DATARAW;
							}
						}
						break;
					}
					default: {
						break;
					}
				}
			}
		}
		if(curspecial < script->numspecials) script->numspecials = curspecial;
		qsort(LUT, script->numspecials, sizeof(struct lut), lutcompare);
		
		for(unsigned i = 0; i < script->numspecials; i++) {
			for(unsigned j = 0; j < LUT[i].numfixup; j++) {
				*(LUT[i].fixup[j]) = script->numsections + i;
			}
			script->specials[i] = LUT[i].data;
		}
		
		for(unsigned i = 0; i < tofree; i++) free(LUT[i].fixup);
		free(LUT);
	}
	
	/* set up section offset table and startwords */
	script->startwords = script->numsections + script->numspecials;
	script->offsettable = malloc(sizeof(uint32_t) * script->numsections);
	curoffset = 4 + script->startwords * 4;
	for(cursec = 0; cursec < script->numsections; cursec++) {
		script->offsettable[cursec] = curoffset;
		curoffset += script->secarr[cursec]->datasize;
	}
}

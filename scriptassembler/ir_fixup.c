#include <stdlib.h>

#include "ir.h"

int findidx(struct ir_special *specials, unsigned valid, uint16_t offset, uint16_t section) {
	for(unsigned i = 0; i < valid; i++) {
		if(specials[i].offset == offset && specials[i].section == section) return i;
	}
	return -1;
}

void ir_script_fixup(struct ir_script *script, unsigned gamenum) {
	uint32_t curoffset;
	uint16_t targetsec, targetoff;
	unsigned cursec, curcmd, curdata, curspecial;
	int idx;
	struct ir_section *section;
	struct ir_generic *command;
	if(script->numspecials) script->specials = malloc(sizeof(struct ir_special) * script->numspecials);
	
	/* find and set up specials area */
	curspecial = 0;
	for(cursec = 0; cursec < script->numsections; cursec++) {
		section = script->secarr[cursec];
		for(curcmd = 0; curcmd < section->numcommands; curcmd++) {
			command = section->commands[curcmd];
			switch(command->type) {
				case CMD35:
				case CMD36: {
					for(curdata = 0; curdata < command->numdata; curdata++) {
						if(command->data[curdata].type == DATASECOFF) {
							//~ script->specials[curspecial].offset = command->data[curdata].data & 0xFFFF;
							//~ script->specials[curspecial].section = (command->data[curdata].data >> 16) & 0xFFFF;
							
							
							targetoff = command->data[curdata].data & 0xFFFF;
							targetsec = (command->data[curdata].data >> 16) & 0xFFFF;
							/* check if we have a usable LUT entry on hand, if so use that */
							if((idx = findidx(script->specials, curspecial, targetoff, targetsec)) > -1) {
								command->data[curdata].data = script->numsections+idx;
							}
							/* otherwise create a new lut entry */
							else {
								script->specials[curspecial].offset = targetoff;
								script->specials[curspecial].section = targetsec;
								command->data[curdata].data = script->numsections+curspecial;
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
	
	/* set up section offset table and startwords */
	script->startwords = script->numsections + script->numspecials;
	script->offsettable = malloc(sizeof(uint32_t) * script->numsections);
	curoffset = 4 + script->startwords * 4;
	for(cursec = 0; cursec < script->numsections; cursec++) {
		script->offsettable[cursec] = curoffset;
		curoffset += script->secarr[cursec]->datasize;
	}
}

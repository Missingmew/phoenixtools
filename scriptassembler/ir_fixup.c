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

int findhash(unsigned long *hashes, unsigned count, unsigned long what) {
	for(unsigned i = 0; i < count; i++) if(hashes[i] == what) {
		printf("%s what %lx found %lx at %u\n", __func__, what, hashes[i], i);
		return i;
	}
	return -1;
}

unsigned ir_script_fixup(struct ir_script *script, unsigned gamenum) {
	unsigned lutsize = 0, curhash = 0, curfixup = 0, curspecial = 0;
	unsigned curoffset;
	unsigned long *hashes;
	unsigned long **fixups;
	int idx;
	struct ir_section *section;
	struct ir_generic *command;
	for(unsigned i = 0; i < script->numsections; i++) lutsize += script->secarr[i]->numlabels;
	if(lutsize) {
		fixups = malloc(sizeof(unsigned long *) * script->numspecials);
		hashes = malloc(sizeof(unsigned long) * lutsize);
		script->specials = malloc(sizeof(struct ir_special) * lutsize);
		/* fetch all labels from all sections, construct LUT data, store addresses which need fixup and store hashes for fixup */
		for(unsigned cursec = 0; cursec < script->numsections; cursec++) {
			section = script->secarr[cursec];
			/* grab labels from section and construct LUT data, storing hash for fixup later */
			for(unsigned curlab = 0; curlab < section->numlabels; curlab++) {
				hashes[curhash++] = section->labels[curlab]->hash;
				script->specials[curspecial].section = cursec;
				script->specials[curspecial++].offset = section->labels[curlab]->addr;
			}
			/* collect stuff needed to fix up commands */
			for(unsigned curcmd = 0; curcmd < section->numcommands; curcmd++) {
				command = section->commands[curcmd];
				switch(command->type) {
					/* rather ugly hack: if label points outside current section fixup flag and fall through, else fixup data and be done */
					case CMD35:
					case CMD36: 
					case CMD78:
					case CMD7A: {
						for(unsigned curdata = 0; curdata < command->numdata; curdata++) {
							if(command->data[curdata].type == DATALOOKUPLAB) {
								fixups[curfixup++] = &command->data[curdata].data;
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
		
		for(unsigned i = 0; i < curfixup; i++) {
			if((idx = findhash(hashes, curhash, *fixups[i])) < 0) {
				printf("fixup (%s): failed to find label for hash %08lx\n", __func__, *fixups[i]);
				return 0;
			}
			else *(fixups[i]) = script->numsections + idx;
		}
		free(fixups);
		free(hashes);
		script->numspecials = lutsize;
	}
	
	/* set up section offset table and startwords */
	script->startwords = script->numsections + script->numspecials;
	script->offsettable = malloc(sizeof(uint32_t) * script->numsections);
	curoffset = 4 + script->startwords * 4;
	for(unsigned cursec = 0; cursec < script->numsections; cursec++) {
		script->offsettable[cursec] = curoffset;
		curoffset += script->secarr[cursec]->datasize;
	}
	return 1;
}

#include <stdlib.h>

#include "ir.h"

struct locallut {
	unsigned long hash;
	unsigned addr;
};

int findhash(unsigned long *hashes, unsigned count, unsigned long what) {
	for(unsigned i = 0; i < count; i++) if(hashes[i] == what) return i;
	return -1;
}

int findlocalhash(struct locallut *hashes, unsigned count, unsigned long what) {
	for(unsigned i = 0; i < count; i++) if(hashes[i].hash == what) return i;
	return -1;
}

unsigned ir_script_fixup(struct ir_script *script) {
	unsigned lutsize = 0, curhash = 0, curfixup = 0, curspecial = 0, curlocal;
	unsigned curoffset;
	unsigned long *hashes;
	unsigned long **fixups;
	int idx;
	struct ir_section *section;
	struct ir_generic *command;
	struct locallut *locals;
	for(unsigned i = 0; i < script->numsections; i++) lutsize += script->secarr[i]->numlabels;
	if(lutsize) {
		if(script->numspecials) {
			fixups = malloc(sizeof(unsigned long *) * script->numspecials);
			script->specials = malloc(sizeof(struct ir_special) * lutsize);
		}
		hashes = malloc(sizeof(unsigned long) * lutsize);
		/* fetch all labels from all sections, construct LUT data, store addresses which need fixup and store hashes for fixup */
		for(unsigned cursec = 0; cursec < script->numsections; cursec++) {
			curlocal = 0;
			section = script->secarr[cursec];
			locals = malloc(sizeof(struct locallut) * section->numlabels);
			/* grab labels from section and construct LUT data, storing hash for fixup later */
			for(unsigned curlab = 0; curlab < section->numlabels; curlab++) {
				if(section->labels[curlab]->name[0] == '.') {
					locals[curlocal].hash = section->labels[curlab]->hash;
					locals[curlocal++].addr = section->labels[curlab]->addr;
					lutsize--;
				}
				else {
					hashes[curhash++] = section->labels[curlab]->hash;
					script->specials[curspecial].section = cursec;
					script->specials[curspecial++].offset = section->labels[curlab]->addr;
				}
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
							if(command->data[curdata].type == DATALOOKUPGLOBAL) {
								fixups[curfixup++] = &command->data[curdata].data;
								command->data[curdata].type = DATARAW;
							}
							else if(command->data[curdata].type == DATALOOKUPLOCAL) {
								if((idx = findlocalhash(locals, curlocal, command->data[curdata].data)) < 0) {
									printf("fixup (%s): failed to find label for hash %08lx\n", __func__, command->data[curdata].data);
									return 0;
								}
								else command->data[curdata].data = locals[idx].addr;
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
			free(locals);
		}
		
		for(unsigned i = 0; i < curfixup; i++) {
			if((idx = findhash(hashes, curhash, *fixups[i])) < 0) {
				printf("fixup (%s): failed to find label for hash %08lx\n", __func__, *fixups[i]);
				return 0;
			}
			else *(fixups[i]) = script->numsections + idx;
		}
		if(script->numspecials) free(fixups);
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

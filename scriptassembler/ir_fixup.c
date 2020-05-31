#include <stdlib.h>

#include "ir.h"

void ir_script_fixup(struct ir_script *script, unsigned gamenum) {
	uint32_t curoffset;
	unsigned cursec, curcmd, curdata, curspecial;
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
							script->specials[curspecial].offset = command->data[curdata].data & 0xFFFF;
							script->specials[curspecial].section = (command->data[curdata].data >> 16) & 0xFFFF;
							command->data[curdata].data = script->numsections+curspecial;
							command->data[curdata].type = DATARAW;
							curspecial++;
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
	
	/* set up section offset table and startwords */
	script->startwords = script->numsections + script->numspecials;
	script->offsettable = malloc(sizeof(uint32_t) * script->numsections);
	curoffset = 4 + script->startwords * 4;
	for(cursec = 0; cursec < script->numsections; cursec++) {
		script->offsettable[cursec] = curoffset;
		curoffset += script->secarr[cursec]->datasize;
	}
}

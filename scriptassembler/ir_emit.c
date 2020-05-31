#include <stdio.h>
#include <stdint.h>

#include "ir.h"

void ir_script_emit(FILE *o, struct ir_script *script) {
	unsigned cursec, curcmd, curdata;
	uint16_t data;
	struct ir_section *section;
	struct ir_generic *command;
	fwrite(&script->startwords, sizeof(uint32_t), 1, o);
	fwrite(script->offsettable, sizeof(uint32_t) * script->numsections, 1, o);
	for(cursec = 0; cursec < script->numsections; cursec++) {
		section = script->secarr[cursec];
		for(curcmd = 0; curcmd < section->numcommands; curcmd++) {
			command = section->commands[curcmd];
			//~ printf("%s: cmd %04x with %u datas\n", __func__, command->type, command->numdata);
			for(curdata = 0; curdata < command->numdata; curdata++) {
				data = command->data[curdata].data;
				fwrite(&data, sizeof(uint16_t), 1, o);
			}
			//~ printf("%s: at %x\n", __func__, ftell(o));
		}
	}
}

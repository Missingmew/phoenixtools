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
	for(unsigned i = 0; i < script->numspecials; i++) {
		fwrite(&script->specials[i], sizeof(struct ir_special), 1, o);
	}
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

void ir_script_emit_header(FILE *h, struct ir_script *script, char *name) {
	fprintf(h, "#ifndef _%s_SCRIPT_GUARD\n", name);
	fprintf(h, "#define _%s_SCRIPT_GUARD\n\n", name);
	{
		for(int cursec = 0; cursec < script->numsections; cursec++) {
			struct ir_section * section = script->secarr[cursec];
			fprintf(h, "#define SCRIPT_%s_S_%s 0x%04X\n", name, section->name, 0x80+cursec);
		}
	}
	fprintf(h, "\n#endif _%s_SCRIPT_GUARD\n", name);
}

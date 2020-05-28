#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "parser.h"
#include "lexer.h"
#include "ir.h"

#include "../phoenixscript_commands.h"

#define CHECKTOKENTYPE(x, caller) if(currenttoken.type != x) { printf("parser (%s): Syntax error at line %u, column %u: expected %s but got %s\n", caller, currenttoken.line, currenttoken.column, ttype < sizeofarr(commandnames) ? commandnames[ttype] : tokentypestrings[ttype-sizeofarr(commandnames)], currenttoken.type < sizeofarr(commandnames) ? commandnames[currenttoken.type] : tokentypestrings[currenttoken.type-sizeofarr(commandnames)]); return 0; }
#define ACCEPT(tok) if(!parser_accept(tok, __func__)) return NULL;
#define ACCEPTRET(ret,tok) if(!(ret = parser_acceptret(tok, __func__))) return NULL;

#define sizeofarr(a) (sizeof(a) / sizeof(a[0]))

#define GENERIC0ARG(token) \
	struct ir_pre_generic *command = malloc(sizeof(struct ir_pre_generic)); \
	command->type = token; \
	command->line = currenttoken.line; \
	command->numdata = 0; \
	command->data = NULL; \
	ACCEPT(token) \
	return command;

#define GENERICNARG(token, count) \
	char *arg; \
	struct ir_pre_generic *command = malloc(sizeof(struct ir_pre_generic)); \
	command->type = token; \
	command->line = currenttoken.line; \
	command->numdata = count; \
	command->data = malloc(sizeof(char *) * count); \
	ACCEPT(token) \
	ACCEPTRET(arg, INTEGER) \
	command->data[0] = arg; \
	for(unsigned i = 1; i < count; i++) { \
		ACCEPT(COMMA) \
		ACCEPTRET(arg, INTEGER) \
		command->data[i] = arg; \
	} \
	return command;
	
#define PREPARENDATA(token, count) \
	struct ir_pre_generic *command = malloc(sizeof(struct ir_pre_generic)); \
	command->type = token; \
	command->line = currenttoken.line; \
	command->numdata = count; \
	command->data = malloc(sizeof(char *) * count); \
	ACCEPT(token)

char *parser_acceptret(tokentype ttype, const char *caller) {
	CHECKTOKENTYPE(ttype, caller)
	char *ret = currenttoken.string;
	return lexer_scan() ? ret : NULL;
}

unsigned int parser_accept(tokentype ttype, const char *caller) {
	CHECKTOKENTYPE(ttype, caller)
	free(currenttoken.string);
	return lexer_scan();
}

struct ir_pre_generic *parser_parseCommand00() {
	GENERIC0ARG(CMD00)
}

struct ir_pre_generic *parser_parseCommand01() {
	GENERIC0ARG(CMD01)
}

struct ir_pre_generic *parser_parseCommand02() {
	GENERIC0ARG(CMD02)
}

struct ir_pre_generic *parser_parseCommand03() {
	char *color;
	PREPARENDATA(CMD03, 1)
	ACCEPTRET(color, IDENT)
	command->data[0] = color;
	return command;
}

struct ir_pre_generic *parser_parseCommand04() {
	GENERIC0ARG(CMD04)
}

struct ir_pre_generic *parser_parseCommand05() {
	char *music, *fade;
	PREPARENDATA(CMD05, 2)
	ACCEPTRET(music, IDENT)
	ACCEPT(COMMA)
	ACCEPTRET(fade, INTEGER)
	command->data[0] = music;
	command->data[1] = fade;
	return command;
}

struct ir_pre_generic *parser_parseCommand06() {
	char *sound, *startstop, *fade;
	PREPARENDATA(CMD06, 2)
	ACCEPTRET(sound, IDENT)
	command->data[0] = sound;
	ACCEPT(COMMA)
	if(currenttoken.type == IDENT) {
		command->type = CMD06_GBA;
		ACCEPTRET(startstop, IDENT)
		command->data[1] = startstop;
	}
	else {
		ACCEPTRET(fade, INTEGER)
		command->data[1] = fade;
	}
	return command;
}

struct ir_pre_generic *parser_parseCommand07() {
	GENERIC0ARG(CMD07)
}

struct ir_pre_generic *parser_parseCommand08() {
	GENERICNARG(CMD08, 2)
}

struct ir_pre_generic *parser_parseCommand09() {
	GENERICNARG(CMD09, 3)
}

struct ir_pre_generic *parser_parseCommand0a() {
	GENERICNARG(CMD0A, 1)
}

struct ir_pre_generic *parser_parseCommand0b() {
	GENERICNARG(CMD0B, 1)
}

struct ir_pre_generic *parser_parseCommand0c() {
	GENERICNARG(CMD0C, 1)
}

struct ir_pre_generic *parser_parseCommand0d() {
	GENERIC0ARG(CMD0D)
}

struct ir_pre_generic *parser_parseCommand0e() {
	char *name, *side;
	PREPARENDATA(CMD0E, 2)
	ACCEPTRET(name, IDENT)
	ACCEPT(COMMA)
	ACCEPTRET(side, IDENT)
	command->data[0] = name;
	command->data[1] = side;
	return command;
}

struct ir_pre_generic *parser_parseCommand0f() {
	char *target1, *hide;
	PREPARENDATA(CMD0F, 2)
	ACCEPTRET(target1, INTEGER)
	ACCEPT(COMMA)
	ACCEPTRET(hide, IDENT)
	command->data[0] = target1;
	command->data[1] = hide;
	return command;
}

struct ir_pre_generic *parser_parseCommand10() {
	GENERICNARG(CMD10, 3)
}

struct ir_pre_generic *parser_parseCommand11() {
	GENERIC0ARG(CMD11)
}

struct ir_pre_generic *parser_parseCommand12() {
	char *delay, *mode, *delta, *target;
	PREPARENDATA(CMD12, 4)
	ACCEPTRET(delay, INTEGER)
	ACCEPT(COMMA)
	ACCEPTRET(mode, IDENT)
	ACCEPT(COMMA)
	ACCEPTRET(delta, INTEGER)
	ACCEPT(COMMA)
	ACCEPTRET(target, INTEGER)
	command->data[0] = delay;
	command->data[1] = mode;
	command->data[2] = delta;
	command->data[3] = target;
	return command;
}

struct ir_pre_generic *parser_parseCommand13() {
	char *id, *side;
	PREPARENDATA(CMD13, 2)
	ACCEPTRET(id, INTEGER)
	ACCEPT(COMMA)
	ACCEPTRET(side, IDENT)
	command->data[0] = id;
	command->data[1] = side;
	return command;
}

struct ir_pre_generic *parser_parseCommand14() {
	GENERIC0ARG(CMD14)
}

struct ir_pre_generic *parser_parseCommand15() {
	GENERIC0ARG(CMD15)
}

struct ir_pre_generic *parser_parseCommand16() {
	GENERIC0ARG(CMD16)
}

struct ir_pre_generic *parser_parseCommand17() {
	GENERICNARG(CMD17, 1)
}

struct ir_pre_generic *parser_parseCommand18() {
	GENERICNARG(CMD18, 1)
}

struct ir_pre_generic *parser_parseCommand19() {
	GENERICNARG(CMD19, 2)
}

struct ir_pre_generic *parser_parseCommand1a() {
	GENERICNARG(CMD1A, 4)
}

struct ir_pre_generic *parser_parseCommand1b() {
	char *bgname;
	PREPARENDATA(CMD1B, 1)
	ACCEPTRET(bgname, IDENT)
	command->data[0] = bgname;
	return command;
}

struct ir_pre_generic *parser_parseCommand1c() {
	GENERICNARG(CMD1C, 1)
}

struct ir_pre_generic *parser_parseCommand1d() {
	char *direction, *speed;
	PREPARENDATA(CMD1D, 2)
	ACCEPTRET(direction, IDENT)
	ACCEPT(COMMA)
	ACCEPTRET(speed, INTEGER)
	command->data[0] = direction;
	command->data[1] = speed;
	return command;
}

struct ir_pre_generic *parser_parseCommand1e() {
	char *person, *arg2, *arg3;
	PREPARENDATA(CMD1E, 3)
	ACCEPTRET(person, IDENT)
	ACCEPT(COMMA)
	ACCEPTRET(arg2, INTEGER)
	ACCEPT(COMMA)
	ACCEPTRET(arg3, INTEGER)
	command->data[0] = person;
	command->data[1] = arg2;
	command->data[2] = arg3;
	return command;
}

struct ir_pre_generic *parser_parseCommand1f() {
	GENERIC0ARG(CMD1F)
}

struct ir_pre_generic *parser_parseCommand20() {
	GENERICNARG(CMD20, 1)
}

struct ir_pre_generic *parser_parseCommand21() {
	GENERIC0ARG(CMD21)
}

struct ir_pre_generic *parser_parseCommand22() {
	char *inout, *fadetime;
	PREPARENDATA(CMD22, 2)
	ACCEPTRET(inout, IDENT)
	ACCEPT(COMMA)
	ACCEPTRET(fadetime, INTEGER)
	command->data[0] = inout;
	command->data[1] = fadetime;
	return command;
}

struct ir_pre_generic *parser_parseCommand23() {
	GENERICNARG(CMD23, 2)
}

struct ir_pre_generic *parser_parseCommand24() {
	GENERIC0ARG(CMD24)
}

struct ir_pre_generic *parser_parseCommand25() {
	GENERICNARG(CMD25, 1)
}

struct ir_pre_generic *parser_parseCommand26() {
	GENERICNARG(CMD26, 1)
}

struct ir_pre_generic *parser_parseCommand27() {
	GENERICNARG(CMD27, 2)
}

struct ir_pre_generic *parser_parseCommand28() {
	GENERICNARG(CMD28, 1)
}

struct ir_pre_generic *parser_parseCommand29() {
	GENERICNARG(CMD29, 1)
}

struct ir_pre_generic *parser_parseCommand2a() {
	GENERICNARG(CMD2A, 3)
}

struct ir_pre_generic *parser_parseCommand2b() {
	GENERIC0ARG(CMD2B)
}

struct ir_pre_generic *parser_parseCommand2c() {
	GENERICNARG(CMD2C, 1)
}

struct ir_pre_generic *parser_parseCommand2d() {
	GENERIC0ARG(CMD2D)
}

struct ir_pre_generic *parser_parseCommand2e() {
	GENERIC0ARG(CMD2E)
}

struct ir_pre_generic *parser_parseCommand2f() {
	char *arg1, *anistate;
	PREPARENDATA(CMD2F, 2)
	ACCEPTRET(arg1, INTEGER)
	ACCEPT(COMMA)
	ACCEPTRET(anistate, IDENT)
	command->data[0] = arg1;
	command->data[1] = anistate;
	return command;
}

struct ir_pre_generic *parser_parseCommand30() {
	GENERICNARG(CMD30, 1)
}

struct ir_pre_generic *parser_parseCommand31() {
	GENERICNARG(CMD31, 2)
}

struct ir_pre_generic *parser_parseCommand32() {
	GENERICNARG(CMD32, 2)
}

struct ir_pre_generic *parser_parseCommand33() {
	char *loc1, *loc2, *loc3, *loc4, *loc5;
	PREPARENDATA(CMD33, 5)
	ACCEPTRET(loc1, IDENT)
	ACCEPT(COMMA)
	ACCEPTRET(loc2, IDENT)
	ACCEPT(COMMA)
	ACCEPTRET(loc3, IDENT)
	ACCEPT(COMMA)
	ACCEPTRET(loc4, IDENT)
	ACCEPT(COMMA)
	ACCEPTRET(loc5, IDENT)
	command->data[0] = loc1;
	command->data[1] = loc2;
	command->data[2] = loc3;
	command->data[3] = loc4;
	command->data[4] = loc5;
	return command;
}

struct ir_pre_generic *parser_parseCommand34() {
	GENERICNARG(CMD34, 1)
}

struct ir_pre_generic *parser_parseCommand35() {
	char *flaghint, *whichflag, *section, *offset;
	PREPARENDATA(CMD35, 4)
	ACCEPTRET(flaghint, IDENT)
	ACCEPT(COMMA)
	ACCEPTRET(whichflag, INTEGER)
	ACCEPT(COMMA)
	ACCEPTRET(section, INTEGER)
	ACCEPT(PLUS)
	ACCEPTRET(offset, INTEGER)
	command->data[0] = flaghint;
	command->data[1] = whichflag;
	command->data[2] = section;
	command->data[3] = offset;
	return command;
}

struct ir_pre_generic *parser_parseCommand36() {
	char *section, *offset;
	PREPARENDATA(CMD36, 2)
	ACCEPTRET(section, INTEGER)
	ACCEPT(PLUS)
	ACCEPTRET(offset, INTEGER)
	command->data[0] = section;
	command->data[1] = offset;
	return command;
}

struct ir_pre_generic *parser_parseCommand37() {
	GENERICNARG(CMD37, 2)
}

struct ir_pre_generic *parser_parseCommand38() {
	GENERICNARG(CMD38, 1)
}

struct ir_pre_generic *parser_parseCommand39() {
	GENERICNARG(CMD39, 1)
}

struct ir_pre_generic *parser_parseCommand3a() {
	GENERIC0ARG(CMD3A)
}

struct ir_pre_generic *parser_parseCommand3b() {
	GENERICNARG(CMD3B, 2)
}

struct ir_pre_generic *parser_parseCommand3c() {
	GENERICNARG(CMD3C, 1)
}

struct ir_pre_generic *parser_parseCommand3d() {
	GENERICNARG(CMD3D, 1)
}

struct ir_pre_generic *parser_parseCommand3e() {
	GENERICNARG(CMD3E, 1)
}

struct ir_pre_generic *parser_parseCommand3f() {
	GENERIC0ARG(CMD3F)
}

struct ir_pre_generic *parser_parseCommand40() {
	GENERIC0ARG(CMD40)
}

struct ir_pre_generic *parser_parseCommand41() {
	GENERIC0ARG(CMD41)
}

struct ir_pre_generic *parser_parseCommand42() {
	GENERICNARG(CMD42, 1)
}

struct ir_pre_generic *parser_parseCommand43() {
	GENERICNARG(CMD43, 1)
}

struct ir_pre_generic *parser_parseCommand44() {
	GENERICNARG(CMD44, 1)
}

struct ir_pre_generic *parser_parseCommand45() {
	GENERIC0ARG(CMD45)
}

struct ir_pre_generic *parser_parseCommand46() {
	GENERICNARG(CMD46, 1)
}

struct ir_pre_generic *parser_parseCommand47() {
	GENERICNARG(CMD47, 2)
}

struct ir_pre_generic *parser_parseCommand48() {
	GENERICNARG(CMD42, 2)
}

struct ir_pre_generic *parser_parseCommand49() {
	GENERIC0ARG(CMD49)
}

struct ir_pre_generic *parser_parseCommand4a() {
	GENERICNARG(CMD4A, 1)
}

struct ir_pre_generic *parser_parseCommand4b() {
	GENERICNARG(CMD4B, 1)
}

struct ir_pre_generic *parser_parseCommand4c() {
	GENERIC0ARG(CMD4C)
}

struct ir_pre_generic *parser_parseCommand4d() {
	GENERICNARG(CMD4D, 2)
}

struct ir_pre_generic *parser_parseCommand4e() {
	GENERICNARG(CMD4E, 1)
}

struct ir_pre_generic *parser_parseCommand4f() {
	GENERICNARG(CMD4F, 7)
}

struct ir_pre_generic *parser_parseCommand50() {
	GENERICNARG(CMD50, 1)
}

struct ir_pre_generic *parser_parseCommand51() {
	GENERICNARG(CMD51, 2)
}

struct ir_pre_generic *parser_parseCommand52() {
	GENERICNARG(CMD52, 1)
}

struct ir_pre_generic *parser_parseCommand53() {
	GENERIC0ARG(CMD53)
}

struct ir_pre_generic *parser_parseCommand54() {
	GENERICNARG(CMD54, 2)
	char *arg1, *arg2;
	ACCEPT(CMD54)
	ACCEPTRET(arg1, INTEGER)
	ACCEPTRET(arg2, INTEGER)
}

struct ir_pre_generic *parser_parseCommand55() {
	GENERICNARG(CMD55, 2)
	char *arg1, *arg2;
	ACCEPT(CMD55)
	ACCEPTRET(arg1, INTEGER)
	ACCEPTRET(arg2, INTEGER)
}

struct ir_pre_generic *parser_parseCommand56() {
	GENERICNARG(CMD56, 2)
	char *arg1, *arg2;
	ACCEPT(CMD56)
	ACCEPTRET(arg1, INTEGER)
	ACCEPTRET(arg2, INTEGER)
}

struct ir_pre_generic *parser_parseCommand57() {
	GENERICNARG(CMD57, 1)
}

struct ir_pre_generic *parser_parseCommand58() {
	GENERIC0ARG(CMD58)
}

struct ir_pre_generic *parser_parseCommand59() {
	GENERICNARG(CMD59, 1)
}

struct ir_pre_generic *parser_parseCommand5a() {
	GENERICNARG(CMD5A, 1)
}

struct ir_pre_generic *parser_parseCommand5b() {
	GENERICNARG(CMD5B, 2)
}

struct ir_pre_generic *parser_parseCommand5c() {
	GENERICNARG(CMD5C, 3)
}

struct ir_pre_generic *parser_parseCommand5d() {
	GENERICNARG(CMD5D, 1)
}

struct ir_pre_generic *parser_parseCommand5e() {
	GENERICNARG(CMD5E, 1)
}

struct ir_pre_generic *parser_parseCommand5f() {
	GENERICNARG(CMD5F, 3)
}

struct ir_pre_generic *parser_parseCommand60() {
	char *arg1, *itemid, *targetgood, *targetbad;
	PREPARENDATA(CMD60, 4)
	ACCEPTRET(arg1, INTEGER)
	ACCEPT(COMMA)
	ACCEPTRET(itemid, INTEGER)
	ACCEPT(COMMA)
	ACCEPTRET(targetgood, INTEGER)
	ACCEPT(COMMA)
	ACCEPTRET(targetbad, INTEGER)
	command->data[0] = arg1;
	command->data[1] = itemid;
	command->data[2] = targetgood;
	command->data[3] = targetbad;
	return command;
}

struct ir_pre_generic *parser_parseCommand61() {
	GENERICNARG(CMD61, 3)
}

struct ir_pre_generic *parser_parseCommand62() {
	GENERIC0ARG(CMD62)
}

struct ir_pre_generic *parser_parseCommand63() {
	GENERIC0ARG(CMD63)
}

struct ir_pre_generic *parser_parseCommand64() {
	GENERICNARG(CMD64, 1)
}

struct ir_pre_generic *parser_parseCommand65() {
	GENERICNARG(CMD65, 2)
}

struct ir_pre_generic *parser_parseCommand66() {
	GENERICNARG(CMD66, 3)
}

struct ir_pre_generic *parser_parseCommand67() {
	GENERIC0ARG(CMD67)
}

struct ir_pre_generic *parser_parseCommand68() {
	GENERIC0ARG(CMD68)
}

struct ir_pre_generic *parser_parseCommand69() {
	GENERICNARG(CMD69, 1)
}

struct ir_pre_generic *parser_parseCommand6a() {
	GENERICNARG(CMD6A, 1)
}

struct ir_pre_generic *parser_parseCommand6b() {
	GENERICNARG(CMD6B, 3)
}

struct ir_pre_generic *parser_parseCommand6c() {
	GENERICNARG(CMD6C, 1)
}

struct ir_pre_generic *parser_parseCommand6d() {
	GENERICNARG(CMD6D, 1)
}

struct ir_pre_generic *parser_parseCommand6e() {
	GENERICNARG(CMD6E, 1)
}

struct ir_pre_generic *parser_parseCommand6f() {
	GENERICNARG(CMD6F, 1)
}

struct ir_pre_generic *parser_parseCommand70() {
	GENERICNARG(CMD70, 3)
}

struct ir_pre_generic *parser_parseCommand71() {
	GENERICNARG(CMD71, 3)
}

struct ir_pre_generic *parser_parseCommand72() {
	GENERIC0ARG(CMD72)
}

struct ir_pre_generic *parser_parseCommand73() {
	GENERIC0ARG(CMD73)
}

struct ir_pre_generic *parser_parseCommand74() {
	GENERICNARG(CMD74, 2)
}

struct ir_pre_generic *parser_parseCommand75() {
	GENERICNARG(CMD75, 3)
}

struct ir_pre_generic *parser_parseCommand76() {
	GENERICNARG(CMD76, 2)
}

struct ir_pre_generic *parser_parseCommand77() {
	GENERICNARG(CMD77, 2)
}

struct ir_pre_generic *parser_parseCommand78() {
	char *section, *offset;
	PREPARENDATA(CMD78, 2)
	ACCEPTRET(section, INTEGER)
	ACCEPT(PLUS)
	ACCEPTRET(offset, INTEGER)
	command->data[0] = section;
	command->data[1] = offset;
	return command;
}

struct ir_pre_generic *parser_parseCommand79() {
	GENERIC0ARG(CMD79)
}

struct ir_pre_generic *parser_parseCommand7a() {
	GENERICNARG(CMD7A, 1)
}

struct ir_pre_generic *parser_parseCommand7b() {
	GENERICNARG(CMD7B, 2)
}

struct ir_pre_generic *parser_parseCommand7c() {
	GENERIC0ARG(CMD7C)
}

struct ir_pre_generic *parser_parseCommand7d() {
	GENERIC0ARG(CMD7D)
}

struct ir_pre_generic *parser_parseCommand7e() {
	GENERIC0ARG(CMD7E)
}

struct ir_pre_generic *parser_parseCommand7f() {
	GENERIC0ARG(CMD7F)
}

struct ir_pre_generic *parser_parseCommand80() {
	GENERIC0ARG(CMD80)
}

struct ir_pre_generic *parser_parseCommand81() {
	GENERIC0ARG(CMD81)
}

struct ir_pre_generic *parser_parseCommand82() {
	GENERIC0ARG(CMD82)
}

struct ir_pre_generic *parser_parseCommand83() {
	GENERIC0ARG(CMD83)
}

struct ir_pre_generic *parser_parseCommand84() {
	GENERIC0ARG(CMD84)
}

struct ir_pre_generic *parser_parseCommand85() {
	GENERIC0ARG(CMD85)
}

struct ir_pre_generic *parser_parseCommand86() {
	GENERIC0ARG(CMD86)
}

struct ir_pre_generic *parser_parseCommand87() {
	GENERIC0ARG(CMD87)
}

struct ir_pre_generic *parser_parseCommand88() {
	GENERIC0ARG(CMD88)
}

struct ir_pre_generic *parser_parseCommand89() {
	GENERIC0ARG(CMD89)
}

struct ir_pre_generic *parser_parseCommand8a() {
	GENERIC0ARG(CMD8A)
}

struct ir_pre_generic *parser_parseCommand8b() {
	GENERIC0ARG(CMD8B)
}

struct ir_pre_generic *parser_parseCommand8c() {
	GENERIC0ARG(CMD8C)
}

struct ir_pre_generic *parser_parseCommand8d() {
	GENERIC0ARG(CMD8D)
}

struct ir_pre_generic *parser_parseCommand8e() {
	GENERIC0ARG(CMD8E)
}

struct ir_pre_generic *parser_parseCommand8f() {
	GENERIC0ARG(CMD8F)
}

struct ir_pre_generic *(*parser_parseCommand[144])() = {
	parser_parseCommand00,
	parser_parseCommand01,
	parser_parseCommand02,
	parser_parseCommand03,
	parser_parseCommand04,
	parser_parseCommand05,
	parser_parseCommand06,
	parser_parseCommand07,
	parser_parseCommand08,
	parser_parseCommand09,
	parser_parseCommand0a,
	parser_parseCommand0b,
	parser_parseCommand0c,
	parser_parseCommand0d,
	parser_parseCommand0e,
	parser_parseCommand0f,
	parser_parseCommand10,
	parser_parseCommand11,
	parser_parseCommand12,
	parser_parseCommand13,
	parser_parseCommand14,
	parser_parseCommand15,
	parser_parseCommand16,
	parser_parseCommand17,
	parser_parseCommand18,
	parser_parseCommand19,
	parser_parseCommand1a,
	parser_parseCommand1b,
	parser_parseCommand1c,
	parser_parseCommand1d,
	parser_parseCommand1e,
	parser_parseCommand1f,
	parser_parseCommand20,
	parser_parseCommand21,
	parser_parseCommand22,
	parser_parseCommand23,
	parser_parseCommand24,
	parser_parseCommand25,
	parser_parseCommand26,
	parser_parseCommand27,
	parser_parseCommand28,
	parser_parseCommand29,
	parser_parseCommand2a,
	parser_parseCommand2b,
	parser_parseCommand2c,
	parser_parseCommand2d,
	parser_parseCommand2e,
	parser_parseCommand2f,
	parser_parseCommand30,
	parser_parseCommand31,
	parser_parseCommand32,
	parser_parseCommand33,
	parser_parseCommand34,
	parser_parseCommand35,
	parser_parseCommand36,
	parser_parseCommand37,
	parser_parseCommand38,
	parser_parseCommand39,
	parser_parseCommand3a,
	parser_parseCommand3b,
	parser_parseCommand3c,
	parser_parseCommand3d,
	parser_parseCommand3e,
	parser_parseCommand3f,
	parser_parseCommand40,
	parser_parseCommand41,
	parser_parseCommand42,
	parser_parseCommand43,
	parser_parseCommand44,
	parser_parseCommand45,
	parser_parseCommand46,
	parser_parseCommand47,
	parser_parseCommand48,
	parser_parseCommand49,
	parser_parseCommand4a,
	parser_parseCommand4b,
	parser_parseCommand4c,
	parser_parseCommand4d,
	parser_parseCommand4e,
	parser_parseCommand4f,
	parser_parseCommand50,
	parser_parseCommand51,
	parser_parseCommand52,
	parser_parseCommand53,
	parser_parseCommand54,
	parser_parseCommand55,
	parser_parseCommand56,
	parser_parseCommand57,
	parser_parseCommand58,
	parser_parseCommand59,
	parser_parseCommand5a,
	parser_parseCommand5b,
	parser_parseCommand5c,
	parser_parseCommand5d,
	parser_parseCommand5e,
	parser_parseCommand5f,
	parser_parseCommand60,
	parser_parseCommand61,
	parser_parseCommand62,
	parser_parseCommand63,
	parser_parseCommand64,
	parser_parseCommand65,
	parser_parseCommand66,
	parser_parseCommand67,
	parser_parseCommand68,
	parser_parseCommand69,
	parser_parseCommand6a,
	parser_parseCommand6b,
	parser_parseCommand6c,
	parser_parseCommand6d,
	parser_parseCommand6e,
	parser_parseCommand6f,
	parser_parseCommand70,
	parser_parseCommand71,
	parser_parseCommand72,
	parser_parseCommand73,
	parser_parseCommand74,
	parser_parseCommand75,
	parser_parseCommand76,
	parser_parseCommand77,
	parser_parseCommand78,
	parser_parseCommand79,
	parser_parseCommand7a,
	parser_parseCommand7b,
	parser_parseCommand7c,
	parser_parseCommand7d,
	parser_parseCommand7e,
	parser_parseCommand7f,
	parser_parseCommand80,
	parser_parseCommand81,
	parser_parseCommand82,
	parser_parseCommand83,
	parser_parseCommand84,
	parser_parseCommand85,
	parser_parseCommand86,
	parser_parseCommand87,
	parser_parseCommand88,
	parser_parseCommand89,
	parser_parseCommand8a,
	parser_parseCommand8b,
	parser_parseCommand8c,
	parser_parseCommand8d,
	parser_parseCommand8e,
	parser_parseCommand8f,
};

struct ir_pre_generic *parser_parseText() {
	char *text;
	PREPARENDATA(TEXT, 1)
	ACCEPTRET(text, STRING)
	command->data[0] = text;
	return command;
}

struct ir_label *parser_parseLabel() {
	char *name;
	struct ir_label *label = malloc(sizeof(struct ir_label));
	label->type = LABEL;
	label->hash = 0;
	label->addr = 0;
	label->name = NULL;
	ACCEPTRET(name, IDENT)
	ACCEPT(COLON)
	label->name = name;
	label->hash = hash(name);
	return label;
}

struct ir_section *parser_parseSection() {
	struct ir_section *section = malloc(sizeof(struct ir_section));
	char *sectionnum;
	
	
	section->type = SECTION;
	section->line = currenttoken.line;
	section->numlabels = 0;
	section->labels = NULL;
	section->numprecommands = 0;
	section->precommands = NULL;
	section->numcommands = 0;
	section->commands = NULL;
	section->prenum = NULL;
	section->num = 0;
	
	ACCEPT(SECTION)
	ACCEPTRET(sectionnum, INTEGER)
	section->prenum = sectionnum;
	
	while(currenttoken.type != ENDSECTION) {
		/* we are inside a section now */
		
		/* for debugging purposes we can have an integer before anything useful, discard it */
		if(currenttoken.type == INTEGER) ACCEPT(INTEGER)
		
		if(currenttoken.type < sizeofarr(commandnames)) {
			if(!ir_section_appendprecommand(section, parser_parseCommand[currenttoken.type]())) return NULL;
		}
		else if(currenttoken.type == TEXT) {
			if(!ir_section_appendprecommand(section, parser_parseText())) return NULL;
		}
		else {
			if(!ir_section_appendlabel(section, parser_parseLabel())) return NULL;
		}
	}
	
	ACCEPT(ENDSECTION)
	return section;
}

struct ir_script *parser_parse() {
	struct ir_script *script = malloc(sizeof(struct ir_script));
	script->numspecials = 0;
	script->specials = NULL;
	script->numsections = 0;
	script->sections = NULL;
	while(currenttoken.type != TEXTEOF) {
		if(!ir_script_appendsection(script, parser_parseSection())) return NULL;
	}
	return script;
}

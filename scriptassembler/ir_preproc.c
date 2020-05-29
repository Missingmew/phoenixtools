#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ir.h"
#include "../phoenixscript_data.h"
#include "../phoenixscript_charsets.h"

#define sizeofarr(a) (sizeof(a) / sizeof(a[0]))

#define SETUPGEN \
	struct ir_generic *gen = malloc(sizeof(struct ir_generic)); \
	gen->type = pre->type; \
	gen->line = pre->line; \
	/* reserve one additional data slot for command itself */ \
	gen->numdata = pre->numdata+1; \
	gen->data = malloc(sizeof(unsigned short) * pre->numdata+1); \
	gen->data[0] = pre->type;

#define PREPROCERR(thing) \
	{ printf("preproc error(%s): lookup for %s in line %u failed\n", __func__, thing, pre->line); return NULL; }

/* take a string of a zero prefixed decimal number and return it as an ushort */
unsigned short cleanNumber(char *str) {
	/* skip any zero as long as we have string after it */
	/* this will leave us with a lone 0 if the entire string was just zeros */
	while(*str == '0' && *str+1) str++;
	return strtoul(str, NULL, 10);
}

/* try to find str within arr[size], return index if found or -1 */
int lookupStr(char **arr, char *str, int size) {
	for(int i = 0; i < size; i++) if(!strcmp(arr[i], str)) return i;
	return -1;
}

#define LOOKUP(idx, data, thing) { \
	idx = lookupStr(data, thing, sizeofarr(data));\
	if(idx < 0) PREPROCERR(thing) }

struct ir_generic *preproc_Generic(struct ir_pre_generic *pre, unsigned gamenum) {
	SETUPGEN
	/* clean up numbers, then assign as data */
	for(unsigned i = 0; i < pre->numdata; i++) gen->data[i+1] = cleanNumber(pre->data[i]);
	
	return gen;
}

struct ir_generic *preproc_Command03(struct ir_pre_generic *pre, unsigned gamenum) {
	int idx;
	SETUPGEN
	LOOKUP(idx, colors, pre->data[0]);
	gen->data[1] = idx;
	return gen;
}

struct ir_generic *preproc_Command05(struct ir_pre_generic *pre, unsigned gamenum) {
	int idx;
	SETUPGEN
	LOOKUP(idx, sound_data[ARRGAMENUM(gamenum)], pre->data[0]);
	gen->data[1] = idx;
	gen->data[2] = cleanNumber(pre->data[1]);
	return gen;
}

struct ir_generic *preproc_Command06(struct ir_pre_generic *pre, unsigned gamenum) {
	if(gamenum == GAME_GS1GBA) {
		int idx;
		SETUPGEN
		LOOKUP(idx, sound_data[ARRGAMENUM(gamenum)], pre->data[0]);
		gen->data[1] = idx;
		LOOKUP(idx, soundplay, pre->data[1]);
		gen->data[2] = idx;
		return gen;
	}
	else return preproc_Command05(pre, gamenum);
}

struct ir_generic *preproc_Command08(struct ir_pre_generic *pre, unsigned gamenum) {
	SETUPGEN
	gen->data[1] = cleanNumber(pre->data[0]) + 128;
	gen->data[2] = cleanNumber(pre->data[1]) + 128;
	return gen;
}

struct ir_generic *preproc_Command09(struct ir_pre_generic *pre, unsigned gamenum) {
	SETUPGEN
	gen->data[1] = cleanNumber(pre->data[0]) + 128;
	gen->data[2] = cleanNumber(pre->data[1]) + 128;
	gen->data[3] = cleanNumber(pre->data[2]) + 128;
	return gen;
}

struct ir_generic *preproc_Command0a(struct ir_pre_generic *pre, unsigned gamenum) {
	SETUPGEN
	gen->data[1] = cleanNumber(pre->data[0]) + 128;
	return gen;
}

struct ir_generic *preproc_Command0e(struct ir_pre_generic *pre, unsigned gamenum) {
	unsigned short data;
	int idx;
	SETUPGEN
	LOOKUP(idx, speakers[ARRGAMENUM(gamenum)], pre->data[0]);
	data = idx << 8;
	LOOKUP(idx, showside, pre->data[1]);
	data += idx;
	gen->data[1] = data;
	return gen;
}

struct ir_generic *preproc_Command0f(struct ir_pre_generic *pre, unsigned gamenum) {
	int idx;
	SETUPGEN
	gen->data[1] = cleanNumber(pre->data[0]) + 128;
	LOOKUP(idx, testimonypress, pre->data[1]);
	gen->data[2] = idx;
	return gen;
}

struct ir_generic *preproc_Command10(struct ir_pre_generic *pre, unsigned gamenum) {
	unsigned short data;
	SETUPGEN
	data = cleanNumber(pre->data[0]) << 8;
	data += cleanNumber(pre->data[1]);
	data |= cleanNumber(pre->data[2]) << 15;
	gen->data[1] = data;
	return gen;
}

struct ir_generic *preproc_Command12(struct ir_pre_generic *pre, unsigned gamenum) {
	unsigned short data;
	int idx;
	SETUPGEN
	data = cleanNumber(pre->data[0]);
	LOOKUP(idx, fademode, pre->data[1]);
	data += idx << 8;
	gen->data[1] = data;
	gen->data[2] = cleanNumber(pre->data[2]);
	gen->data[3] = cleanNumber(pre->data[3]);
	return gen;
}

struct ir_generic *preproc_Command13(struct ir_pre_generic *pre, unsigned gamenum) {
	unsigned short data;
	int idx;
	SETUPGEN
	data = cleanNumber(pre->data[0]);
	LOOKUP(idx, showside, pre->data[1]);
	data += idx << 8;
	gen->data[1] = data;
	return gen;
}

struct ir_generic *preproc_Command1b(struct ir_pre_generic *pre, unsigned gamenum) {
	int idx;
	SETUPGEN
	LOOKUP(idx, backgrounds[ARRGAMENUM(gamenum)], pre->data[0]);
	gen->data[1] = idx;
	return gen;
}

struct ir_generic *preproc_Command1d(struct ir_pre_generic *pre, unsigned gamenum) {
	unsigned short data;
	int idx;
	SETUPGEN
	LOOKUP(idx, shiftdirection, pre->data[0]);
	data = idx << 8;
	data += cleanNumber(pre->data[1]);
	gen->data[1] = data;
	return gen;
}

struct ir_generic *preproc_Command1e(struct ir_pre_generic *pre, unsigned gamenum) {
	int idx;
	SETUPGEN
	LOOKUP(idx, speakers[ARRGAMENUM(gamenum)], pre->data[0]);
	gen->data[1] = idx;
	gen->data[2] = cleanNumber(pre->data[1]);
	gen->data[3] = cleanNumber(pre->data[2]);
	return gen;
}

struct ir_generic *preproc_Command22(struct ir_pre_generic *pre, unsigned gamenum) {
	int idx;
	SETUPGEN
	LOOKUP(idx, musicfading, pre->data[0]);
	gen->data[1] = idx;
	gen->data[2] = cleanNumber(pre->data[1]);
	return gen;
}

struct ir_generic *preproc_Command2f(struct ir_pre_generic *pre, unsigned gamenum) {
	int idx;
	SETUPGEN
	gen->data[1] = cleanNumber(pre->data[0]);
	LOOKUP(idx, animationstate, pre->data[1]);
	gen->data[2] = idx;
	return gen;
}

struct ir_generic *preproc_Command33(struct ir_pre_generic *pre, unsigned gamenum) {
	int idx;
	SETUPGEN
	LOOKUP(idx, musicfading, pre->data[0]); gen->data[1] = idx;
	LOOKUP(idx, musicfading, pre->data[1]); gen->data[2] = idx;
	LOOKUP(idx, musicfading, pre->data[2]); gen->data[3] = idx;
	LOOKUP(idx, musicfading, pre->data[3]); gen->data[4] = idx;
	LOOKUP(idx, musicfading, pre->data[4]); gen->data[5] = idx;
	return gen;
}

/* this needs additional work due to how special data is handled */
struct ir_generic *preproc_Command35(struct ir_pre_generic *pre, unsigned gamenum) {
	return NULL;
}

/* this needs additional work due to how special data is handled */
struct ir_generic *preproc_Command36(struct ir_pre_generic *pre, unsigned gamenum) {
	return NULL;
}

struct ir_generic *preproc_Command60(struct ir_pre_generic *pre, unsigned gamenum) {
	SETUPGEN
	gen->data[1] = cleanNumber(pre->data[0]);
	gen->data[2] = cleanNumber(pre->data[1]);
	gen->data[3] = cleanNumber(pre->data[2])+128;
	gen->data[4] = cleanNumber(pre->data[3])+128;
	return gen;
}

struct ir_generic *preproc_Command78(struct ir_pre_generic *pre, unsigned gamenum) {
	return preproc_Command36(pre, gamenum);
}

struct ir_generic *(*command_preproc[144])(struct ir_pre_generic *pre, unsigned gamenum) = {
	preproc_Generic, /* 0x00 */
	preproc_Generic, /* 0x01 */
	preproc_Generic, /* 0x02 */
	preproc_Command03, /* 0x03 */
	preproc_Generic, /* 0x04 */
	preproc_Command05, /* 0x05 */
	preproc_Command06, /* 0x06 */
	preproc_Generic, /* 0x07 */
	preproc_Command08, /* 0x08 */
	preproc_Command09, /* 0x09 */
	preproc_Command0a, /* 0x0a */
	preproc_Generic, /* 0x0b */
	preproc_Generic, /* 0x0c */
	preproc_Generic, /* 0x0d */
	preproc_Command0e, /* 0x0e */
	preproc_Command0f, /* 0x0f */
	preproc_Command10, /* 0x10 */
	preproc_Generic, /* 0x11 */
	preproc_Command12, /* 0x12 */
	preproc_Command13, /* 0x13 */
	preproc_Generic, /* 0x14 */
	preproc_Generic, /* 0x15 */
	preproc_Generic, /* 0x16 */
	preproc_Generic, /* 0x17 */
	preproc_Generic, /* 0x18 */
	preproc_Generic, /* 0x19 */
	preproc_Generic, /* 0x1a */
	preproc_Command1b, /* 0x1b */
	preproc_Generic, /* 0x1c */
	preproc_Command1d, /* 0x1d */
	preproc_Command1e, /* 0x1e */
	preproc_Generic, /* 0x1f */
	preproc_Generic, /* 0x20 */
	preproc_Generic, /* 0x21 */
	preproc_Command22, /* 0x22 */
	preproc_Generic, /* 0x23 */
	preproc_Generic, /* 0x24 */
	preproc_Generic, /* 0x25 */
	preproc_Generic, /* 0x26 */
	preproc_Generic, /* 0x27 */
	preproc_Generic, /* 0x28 */
	preproc_Generic, /* 0x29 */
	preproc_Generic, /* 0x2a */
	preproc_Generic, /* 0x2b */
	preproc_Generic, /* 0x2c */
	preproc_Generic, /* 0x2d */
	preproc_Generic, /* 0x2e */
	preproc_Command2f, /* 0x2f */
	preproc_Generic, /* 0x30 */
	preproc_Generic, /* 0x31 */
	preproc_Generic, /* 0x32 */
	preproc_Command33, /* 0x33 */
	preproc_Generic, /* 0x34 */
	preproc_Command35, /* 0x35 */
	preproc_Command36, /* 0x36 */
	preproc_Generic, /* 0x37 */
	preproc_Generic, /* 0x38 */
	preproc_Generic, /* 0x39 */
	preproc_Generic, /* 0x3a */
	preproc_Generic, /* 0x3b */
	preproc_Generic, /* 0x3c */
	preproc_Generic, /* 0x3d */
	preproc_Generic, /* 0x3e */
	preproc_Generic, /* 0x3f */
	preproc_Generic, /* 0x40 */
	preproc_Generic, /* 0x41 */
	preproc_Generic, /* 0x42 */
	preproc_Generic, /* 0x43 */
	preproc_Generic, /* 0x44 */
	preproc_Generic, /* 0x45 */
	preproc_Generic, /* 0x46 */
	preproc_Generic, /* 0x47 */
	preproc_Generic, /* 0x48 */
	preproc_Generic, /* 0x49 */
	preproc_Generic, /* 0x4a */
	preproc_Generic, /* 0x4b */
	preproc_Generic, /* 0x4c */
	preproc_Generic, /* 0x4d */
	preproc_Generic, /* 0x4e */
	preproc_Generic, /* 0x4f */
	preproc_Generic, /* 0x50 */
	preproc_Generic, /* 0x51 */
	preproc_Generic, /* 0x52 */
	preproc_Generic, /* 0x53 */
	preproc_Generic, /* 0x54 */
	preproc_Generic, /* 0x55 */
	preproc_Generic, /* 0x56 */
	preproc_Generic, /* 0x57 */
	preproc_Generic, /* 0x58 */
	preproc_Generic, /* 0x59 */
	preproc_Generic, /* 0x5a */
	preproc_Generic, /* 0x5b */
	preproc_Generic, /* 0x5c */
	preproc_Generic, /* 0x5d */
	preproc_Generic, /* 0x5e */
	preproc_Generic, /* 0x5f */
	preproc_Command60, /* 0x60 */
	preproc_Generic, /* 0x61 */
	preproc_Generic, /* 0x62 */
	preproc_Generic, /* 0x63 */
	preproc_Generic, /* 0x64 */
	preproc_Generic, /* 0x65 */
	preproc_Generic, /* 0x66 */
	preproc_Generic, /* 0x67 */
	preproc_Generic, /* 0x68 */
	preproc_Generic, /* 0x69 */
	preproc_Generic, /* 0x6a */
	preproc_Generic, /* 0x6b */
	preproc_Generic, /* 0x6c */
	preproc_Generic, /* 0x6d */
	preproc_Generic, /* 0x6e */
	preproc_Generic, /* 0x6f */
	preproc_Generic, /* 0x70 */
	preproc_Generic, /* 0x71 */
	preproc_Generic, /* 0x72 */
	preproc_Generic, /* 0x73 */
	preproc_Generic, /* 0x74 */
	preproc_Generic, /* 0x75 */
	preproc_Generic, /* 0x76 */
	preproc_Generic, /* 0x77 */
	preproc_Command78, /* 0x78 */
	preproc_Generic, /* 0x79 */
	preproc_Generic, /* 0x7a */
	preproc_Generic, /* 0x7b */
	preproc_Generic, /* 0x7c */
	preproc_Generic, /* 0x7d */
	preproc_Generic, /* 0x7e */
	preproc_Generic, /* 0x7f */
	preproc_Generic, /* 0x80 */
	preproc_Generic, /* 0x81 */
	preproc_Generic, /* 0x82 */
	preproc_Generic, /* 0x83 */
	preproc_Generic, /* 0x84 */
	preproc_Generic, /* 0x85 */
	preproc_Generic, /* 0x86 */
	preproc_Generic, /* 0x87 */
	preproc_Generic, /* 0x88 */
	preproc_Generic, /* 0x89 */
	preproc_Generic, /* 0x8a */
	preproc_Generic, /* 0x8b */
	preproc_Generic, /* 0x8c */
	preproc_Generic, /* 0x8d */
	preproc_Generic, /* 0x8e */
	preproc_Generic, /* 0x8f */
};

struct ir_generic *text_preproc(struct ir_pre_generic *pre, unsigned gamenum) {
	return NULL;
}

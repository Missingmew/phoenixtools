#ifndef PHOENIXSCRIPT_CHARSETS_H
#define PHOENIXSCRIPT_CHARSETS_H

#include <stdint.h>

enum tokenset {
	SET_NONE,
	SET_SHARED,
	SET_DEFAULT,
	SET_EXTENDED
};

extern char *charset_shared[256];
extern char *charset_default[2][256];
extern char *charset_japanese_extended[4][0x500];

enum tokenset charset_isTokenValid(uint16_t token, unsigned isjp, unsigned gamenum);

#endif

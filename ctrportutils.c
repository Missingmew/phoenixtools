#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include "ctrportutils.h"

/*
based on code from https://github.com/FanTranslatorsInternational/Kuriimu2/blob/dev/plugins/Capcom/plugin_capcom/Archives/AAPackSupport.cs
licensed under GPL3
*/

unsigned getseed(unsigned pos, unsigned len) {
	unsigned leastbit = (pos < len-1) ? ~(len-pos) & 1 : 0;
	unsigned seed = leastbit ? 0x1F : 1;
	while(len-pos-1 > leastbit) {
		leastbit += 2;
		seed *= 0x3C1;
	}
	return seed;
}

uint32_t hashstring(char *str) {
	uint32_t res = 0;
	unsigned len, pos;
	len = strlen(str);
	for(pos = 0; pos < len; pos++) {
		res += toupper(str[pos])*getseed(pos, len);
	}
	return res;
}
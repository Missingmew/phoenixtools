#ifndef PHOENIXSCRIPT_COMMANDS_H
#define PHOENIXSCRIPT_COMMANDS_H

#include <stdint.h>
#include <string.h>

typedef struct special {
	uint16_t offset;
	uint16_t section;
}__attribute__((packed)) jumplutpack;


struct localjumpinfo {
	unsigned index;
	unsigned target;
};

struct scriptstate {
	unsigned gamenum;
	unsigned isjp;
	unsigned isunity;
	unsigned textidx;
	char *textfile;
	unsigned maxtext;
	unsigned scriptidx;
	uint16_t *script;
	unsigned scriptsize;
	unsigned outidx;
	char *outbuf;
	unsigned maxoutbuf;
	
	unsigned section;
	unsigned sectionoff;
	uint32_t *sectionlist;
	unsigned numsections;
	jumplutpack *jumplut;
	unsigned numjumplut;
	
	struct localjumpinfo *localjumps;
	unsigned numlocaljumps;
	
	unsigned textstart;
	
	/* this will disable output related functionality in print functions
	   use to just get argument count */
	unsigned outputenabled;
};

extern char *commandnames[144];
extern unsigned (*printcommands[0x90])(struct scriptstate *); // 144

int isSectionStart(uint32_t *list, unsigned count, unsigned index);

int isLabelLocation(jumplutpack *lut, unsigned count, unsigned section, unsigned offset);

int isLocalLabelLocation(struct localjumpinfo *jumps, unsigned count, unsigned section, unsigned offset);

#endif

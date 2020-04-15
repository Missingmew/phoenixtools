#ifndef PHOENIXSCRIPT_COMMANDS_H
#define PHOENIXSCRIPT_COMMANDS_H

#include <stdint.h>
#include <string.h>

typedef struct special {
	uint16_t val0;
	uint16_t val1;
}__attribute__((packed)) specialdatapack;

struct scriptstate {
	unsigned gamenum;
	unsigned isjp;
	unsigned textidx;
	char *textfile;
	unsigned maxtext;
	unsigned scriptidx;
	uint16_t *script;
	unsigned scriptsize;
	unsigned outidx;
	char *outbuf;
	
	unsigned section;
	unsigned sectionoff;
	uint32_t *sectionlist;
	unsigned numsections;
	specialdatapack *specialdata;
	unsigned numspecialdata;
	
	unsigned textstart;
	
	/* this will disable output related functionality in print functions
	   use to just get argument count */
	unsigned outputenabled;
};

typedef struct cmd {
	unsigned (*print)(struct scriptstate *);
	char name[32];
} command;

extern command commands[0x90]; // 144

#endif

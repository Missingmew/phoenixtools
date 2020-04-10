#ifndef PHOENIXSCRIPT_COMMANDS_H
#define PHOENIXSCRIPT_COMMANDS_H

#include <stdint.h>
#include <string.h>

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
	uint16_t *specialdata;
	unsigned numspecialdata;
	
	unsigned textstart;
};

typedef struct cmd {
	char name[32];
	void (*print)(struct scriptstate *);
} command;

extern command commands[144];

int command_tokenFromString(char *);

#endif

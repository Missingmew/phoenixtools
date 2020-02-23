#ifndef PHOENIXSCRIPT_COMMANDS_H
#define PHOENIXSCRIPT_COMMANDS_H

#include <stdint.h>
#include <string.h>

struct scriptstate {
	unsigned gamenum;
	unsigned textidx;
	char *textfile;
	unsigned maxtext;
	unsigned scriptidx;
	uint16_t *script;
	unsigned scriptsize;
	unsigned outidx;
	char *outbuf;
};

struct parsestate {
	unsigned gamenum;
	char *line;
	unsigned scriptidx;
	uint16_t *script;
	unsigned maxscript;
	unsigned sectionidx;
	uint32_t *section;
	unsigned maxsection;
};

typedef struct cmd {
	char name[32];
	void (*print)(struct scriptstate *);
	unsigned (*parse)(struct parsestate *);
} command;

extern command commands[144];

int command_tokenFromString(char *);

#endif

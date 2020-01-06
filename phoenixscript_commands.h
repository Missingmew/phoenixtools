#ifndef PHOENIXSCRIPT_COMMANDS_H
#define PHOENIXSCRIPT_COMMANDS_H

#include <stdint.h>

/* this has to be provided and set by main */
extern unsigned int gamenum;

struct scriptstate {
	unsigned textidx;
	unsigned memidx;
	char *textfile;
	unsigned maxtext;
	uint16_t *script;
	unsigned scriptsize;
};

typedef struct cmd {
	char name[32];
	void (*print)(struct scriptstate *);
} command;

extern command commands[144];

#endif

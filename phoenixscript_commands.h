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

//~ typedef struct cmd {
	//~ unsigned (*print)(struct scriptstate *);
	//~ char *name;
//~ } command;

extern char *commandnames[144];
extern unsigned (*printcommands[0x90])(struct scriptstate *); // 144

enum supportedgames {
	GAME_PHOENIX1,
	GAME_PHOENIX2,
	GAME_PHOENIX3,
	GAME_APOLLO,
	GAME_GS1GBA,
	GAME_NUMGAMES
};

/* this is incredibly dirty but the alternatives would be either a giant waste of space or spaghetti */
#define ARRGAMENUM(x) ((x) % 4)
#define ISNDS(x) ((x) >= GAME_PHOENIX1 && (x) < GAME_GS1GBA)
#define ISGBA(x) ((x) >= GAME_GS1GBA && (x) < GAME_NUMGAMES)

int isSectionStart(uint32_t *list, unsigned count, unsigned index);

int isLabelLocation(jumplutpack *lut, unsigned count, unsigned section, unsigned offset);

int isLocalLabelLocation(struct localjumpinfo *jumps, unsigned count, unsigned section, unsigned offset);

#endif

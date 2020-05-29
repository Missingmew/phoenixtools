#ifndef IR_H_
#define IR_H_

#include "lexer.h"

enum irtypes {
	LABEL = NUMTOKENS+1,
};

struct ir_list {
	struct ir_list *next;
	unsigned int *type;
};

struct ir_pre_generic {
	unsigned type;
	unsigned line;
	unsigned numdata;
	char **data;
};

struct ir_generic {
	unsigned type;
	unsigned line;
	unsigned numdata;
	unsigned short *data;
};

struct ir_label {
	unsigned type;
	unsigned long hash;
	char *name;
	unsigned addr;
};

struct ir_section {
	unsigned type;
	unsigned line;
	
	char *prenum;
	unsigned num;
	
	unsigned datasize;
	
	unsigned numlabels;
	struct ir_label **labels;
	
	unsigned numprecommands;
	struct ir_list *precommands;
	
	unsigned numcommands;
	struct ir_generic **commands;
};

struct ir_script {
	unsigned numspecials;
	struct ir_special *specials;
	
	unsigned numsections;
	struct ir_list *sections;
};

extern struct ir_generic *(*command_preproc[144])(struct ir_pre_generic *pre, unsigned gamenum);
struct ir_generic *text_preproc(struct ir_pre_generic *pre, unsigned gamenum);

unsigned long hash(void *data);

unsigned ir_section_appendprecommand(struct ir_section *section, struct ir_pre_generic *precommand);

unsigned ir_script_appendsection(struct ir_script *script, struct ir_section *section);

void ir_script_dump(struct ir_script *script);

void ir_script_free(struct ir_script *script);

unsigned ir_script_preprocess(struct ir_script *script, unsigned gamenum);

#endif
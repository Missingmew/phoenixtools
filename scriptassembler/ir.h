#ifndef IR_H_
#define IR_H_

#include "lexer.h"

enum irtypes {
	LABEL = NUMTOKENS+1,
	CMD06_GBA,
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
	
	unsigned numlabels;
	struct ir_list *labels;
	
	unsigned numprecommands;
	struct ir_list *precommands;
	
	unsigned numcommands;
	struct ir_list *commands;
};

struct ir_script {
	unsigned numspecials;
	struct ir_special *specials;
	
	unsigned numsections;
	struct ir_list *sections;
};

unsigned long hash(void *data);

int ir_sort_sections(const void *a, const void *b);

unsigned ir_section_appendprecommand(struct ir_section *section, struct ir_pre_generic *precommand);
unsigned ir_section_appendcommand(struct ir_section *section, struct ir_generic *command);
unsigned ir_section_appendlabel(struct ir_section *section, struct ir_label *label);

unsigned ir_script_appendsection(struct ir_script *script, struct ir_section *section);

void ir_pre_generic_dump(struct ir_pre_generic *generic);
void ir_generic_dump(struct ir_generic *generic);
void ir_label_dump(struct ir_label *label);
void ir_section_dump(struct ir_section *section);
void ir_script_dump(struct ir_script *script);

void ir_pre_generic_free(struct ir_pre_generic *generic);
void ir_generic_free(struct ir_generic *generic);
void ir_label_free(struct ir_label *label);
void ir_section_free(struct ir_section *section);
void ir_script_free(struct ir_script *script);

#endif
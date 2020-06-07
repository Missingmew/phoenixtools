#ifndef IR_H_
#define IR_H_

#include <stdio.h>
#include <stdint.h>

#include "lexer.h"
#include "asconfig.h"

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

enum datapackettype {
	DATARAW,
	DATALOOKUPLAB,
};

struct ir_datapacket {
	unsigned type;
	unsigned long data;
};

struct ir_generic {
	unsigned type;
	unsigned line;
	unsigned numdata;
	struct ir_datapacket *data;
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

struct ir_special {
	uint16_t offset;
	uint16_t section;
}__attribute__((packed));

struct ir_script {
	uint32_t startwords;
	uint32_t *offsettable;
	
	unsigned numspecials;
	struct ir_special *specials;
	
	unsigned numsections;
	struct ir_list *sections;
	struct ir_section **secarr;
};

extern int currentsection;
extern unsigned currentspecials;

void ir_script_emit(FILE *o, struct ir_script *script);

unsigned ir_script_fixup(struct ir_script *script);

unsigned cleanNumber(char *str);
extern struct ir_generic *(*command_preproc[144])(struct ir_pre_generic *pre, struct asconfig *config);
struct ir_generic *text_preproc(struct ir_pre_generic *pre, struct asconfig *config);

unsigned long hash(void *data);

unsigned ir_section_appendprecommand(struct ir_section *section, struct ir_pre_generic *precommand);

unsigned ir_script_appendsection(struct ir_script *script, struct ir_section *section);

void ir_script_dump(struct ir_script *script);

void ir_script_free(struct ir_script *script);

unsigned ir_script_preprocess(struct ir_script *script, struct asconfig *config);

#endif

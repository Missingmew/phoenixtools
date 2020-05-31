#include <stdio.h>
#include <stdlib.h>

#include "ir.h"
#include "../phoenixscript_commands.h"

#define sizeofarr(a) (sizeof(a) / sizeof(a[0]))

int currentsection = -1;
unsigned currentspecials = 0;

/* shamelessly stolen from http://www.cse.yorku.ca/~oz/hash.html */
unsigned long hash(void *data) {
	unsigned char *str = data;
	unsigned long hash = 5381;
	unsigned c;
	while((c = *str++)) hash = hash * 33 ^ c;
	return hash;
}

unsigned ir_section_appendprecommand(struct ir_section *section, struct ir_pre_generic *precommand) {
	if(!section || !precommand) return 0;
	if(section->numprecommands == 0) {
		section->precommands = malloc(sizeof(struct ir_list));
		section->precommands->next = NULL;
		section->precommands->type = (unsigned int *)precommand;
	}
	else {
		struct ir_list *list = section->precommands, *new = malloc(sizeof(struct ir_list));
		new->next = NULL;
		new->type = (unsigned int *)precommand;
		for(unsigned i = 0; i < section->numprecommands-1; i++) list = list->next;
		list->next = new;
	}
	if(precommand->type == LABEL) section->numlabels++;
	else section->numcommands++;
	section->numprecommands++;
	return 1;
}

unsigned ir_script_appendsection(struct ir_script *script, struct ir_section *section) {
	if(!script || !section) return 0;
	if(script->numsections == 0) {
		script->sections = malloc(sizeof(struct ir_list));
		script->sections->next = NULL;
		script->sections->type = (unsigned int *)section;
	}
	else {
		struct ir_list *list = script->sections, *new = malloc(sizeof(struct ir_list));
		new->next = NULL;
		new->type = (unsigned int *)section;
		for(unsigned i = 0; i < script->numsections-1; i++) list = list->next;
		list->next = new;
	}
	script->numsections++;
	return 1;
}

void ir_pre_generic_dump(struct ir_pre_generic *generic) {
	printf("pregeneric type %s at line %u has %u datas\n", generic->type < sizeofarr(commandnames) ? commandnames[generic->type] : tokentypestrings[generic->type-sizeofarr(commandnames)], generic->line, generic->numdata);
	printf("datas are: ");
	for(unsigned i = 0; i < generic->numdata; i++) printf("%s - ", generic->data[i]);
	printf("\n");
}

void ir_generic_dump(struct ir_generic *generic) {
	printf("generic type %s at line %u has %u datas\n", generic->type < sizeofarr(commandnames) ? commandnames[generic->type] : tokentypestrings[generic->type-sizeofarr(commandnames)], generic->line, generic->numdata);
	printf("datas are: ");
	for(unsigned i = 0; i < generic->numdata; i++) {
		printf("(%x - %04x) - ", generic->data[i].type, generic->data[i].data);
	}
	printf("\n");
}

void ir_label_dump(struct ir_label *label) {
	printf("Label %s hash %lu addr %u\n", label->name, label->hash, label->addr);
}

void ir_section_dump(struct ir_section *section) {
	unsigned i;
	struct ir_list *iter;
	printf("Section was on line %u, prenum is %s num is %u\n", section->line, section->prenum, section->num);
	printf("Section has %u labels, %u precommands, %u commands\n", section->numlabels, section->numprecommands, section->numcommands);
	if(section->labels) {
		for(i = 0; i < section->numlabels; i++) {
			printf("Dumping label %u\n", i);
			ir_label_dump(section->labels[i]);
		}
	}
	
	for(i = 0, iter = section->precommands; i < section->numprecommands; i++, iter = iter->next) {
		printf("Dumping precommand %u\n", i);
		ir_pre_generic_dump((struct ir_pre_generic *)iter->type);
	}
	
	if(section->commands) {
		for(i = 0; i < section->numcommands; i++) {
			printf("Dumping command %u\n", i);
			ir_generic_dump(section->commands[i]);
		}
	}
}

void ir_script_dump(struct ir_script *script) {
	unsigned i;
	//~ struct ir_special *special;
	struct ir_list *iter;
	
	printf("Script has %u specials and %u sections\n", script->numspecials, script->numsections);
	for(i = 0, iter = script->sections; i < script->numsections; i++, iter = iter->next) {
		printf("Dumping section %u\n", i);
		ir_section_dump((struct ir_section *)iter->type);
	}
}

void ir_pre_generic_free(struct ir_pre_generic *generic) {
	for(unsigned i = 0; i < generic->numdata; i++) free(generic->data[i]);
	free(generic->data);
	free(generic);
}

void ir_generic_free(struct ir_generic *generic) {
	free(generic->data);
	free(generic);
}

void ir_label_free(struct ir_label *label) {
	free(label->name);
	free(label);
}

void ir_section_free(struct ir_section *section) {
	unsigned i;
	struct ir_list *iter, *olditer;
	
	for(i = 0; i < section->numlabels; i++) {
		ir_label_free(section->labels[i]);
	}
	free(section->labels);
	
	for(i = 0, iter = section->precommands, olditer = NULL; i < section->numprecommands; i++, olditer = iter, iter = iter->next) {
		free(olditer);
		ir_pre_generic_free((struct ir_pre_generic *)iter->type);
	}
	free(olditer);
	
	for(i = 0; i < section->numcommands; i++) {
		ir_generic_free(section->commands[i]);
	}
	free(section->commands);
	
	free(section->prenum);
	free(section);
}

void ir_script_free(struct ir_script *script) {
	unsigned i;
	struct ir_list *iter, *olditer;
	if(script->numspecials) free(script->specials);
	for(i = 0, iter = script->sections, olditer = NULL;i < script->numsections; i++, olditer = iter, iter = iter->next) {
		free(olditer);
		ir_section_free((struct ir_section *)iter->type);
	}
	free(olditer);
	free(script);
}

struct ir_generic *ir_precommand_preprocess(struct ir_pre_generic *pre, unsigned gamenum) {
	if(pre->type < sizeofarr(commandnames)) return command_preproc[pre->type](pre, gamenum);
	else if(pre->type == TEXT) return text_preproc(pre, gamenum);
	else {
		printf("error during preprocessing: encountered unknown type %u\n", pre->type);
		return NULL;
	}
}

struct ir_label *ir_label_preprocess(struct ir_label *lab, unsigned cursecaddr) {
	lab->hash = hash(lab->name);
	lab->addr = cursecaddr;
	return lab;
}

unsigned ir_section_preprocess(struct ir_section *section, unsigned gamenum) {
	unsigned i, curcmd, curlab;
	struct ir_list *iter;
	
	section->num = cleanNumber(section->prenum);
	currentsection = section->num;
	
	section->commands = malloc(sizeof(struct ir_generic *) * section->numcommands);
	section->labels = malloc(sizeof(struct ir_label *) * section->numlabels);
	
	for(i = 0, curcmd = 0, curlab = 0, iter = section->precommands;i < section->numprecommands; i++, iter = iter->next) {
		if(*iter->type == LABEL) {
			section->labels[curlab++] = ir_label_preprocess((struct ir_label *)iter->type, section->datasize);
		}
		else {
			if(!(section->commands[curcmd] = ir_precommand_preprocess((struct ir_pre_generic *)iter->type, gamenum))) return 0;
			section->datasize += sizeof(uint16_t) * section->commands[curcmd++]->numdata;
		}
	}
	
	currentsection = -1;
	
	return 1;
}

unsigned ir_script_preprocess(struct ir_script *script, unsigned gamenum) {
	unsigned i;
	struct ir_list *iter;
	for(i = 0, iter = script->sections;i < script->numsections; i++, iter = iter->next) {
		if(!ir_section_preprocess((struct ir_section *)iter->type, gamenum)) return 0;
	}
	script->numspecials = currentspecials;
	return 1;
}

void ir_script_postprocess(struct ir_script *script, unsigned gamenum) {
	
}

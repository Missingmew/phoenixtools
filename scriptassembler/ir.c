#include <stdio.h>
#include <stdlib.h>

#include "ir.h"
#include "../phoenixscript_commands.h"

#define sizeofarr(a) (sizeof(a) / sizeof(a[0]))

/* shamelessly stolen from http://www.cse.yorku.ca/~oz/hash.html */
unsigned long hash(void *data) {
	unsigned char *str = data;
	unsigned long hash = 5381;
	unsigned c;
	while((c = *str++)) hash = hash * 33 ^ c;
	return hash;
}

int ir_sort_sections(const void *a, const void *b) {
	const struct ir_section *left = (const struct ir_section *)a;
	const struct ir_section *right = (const struct ir_section *)b;
	return left->num - right->num;
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
	section->numprecommands++;
	return 1;
}

unsigned ir_section_appendcommand(struct ir_section *section, struct ir_generic *command) {
	if(!section || !command) return 0;
	if(section->numcommands == 0) {
		section->commands = malloc(sizeof(struct ir_list));
		section->commands->next = NULL;
		section->commands->type = (unsigned int *)command;
	}
	else {
		struct ir_list *list = section->commands, *new = malloc(sizeof(struct ir_list));
		new->next = NULL;
		new->type = (unsigned int *)command;
		for(unsigned i = 0; i < section->numcommands-1; i++) list = list->next;
		list->next = new;
	}
	section->numcommands++;
	return 1;
}

unsigned ir_section_appendlabel(struct ir_section *section, struct ir_label *label) {
	if(!section || !label) return 0;
	if(section->numlabels == 0) {
		section->labels = malloc(sizeof(struct ir_list));
		section->labels->next = NULL;
		section->labels->type = (unsigned int *)label;
	}
	else {
		struct ir_list *list = section->labels, *new = malloc(sizeof(struct ir_list));
		new->next = NULL;
		new->type = (unsigned int *)label;
		for(unsigned i = 0; i < section->numlabels-1; i++) list = list->next;
		list->next = new;
	}
	section->numlabels++;
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
	printf("%s is a stub!\n", __func__);
}

void ir_label_dump(struct ir_label *label) {
	printf("Label %s hash %lu addr %u\n", label->name, label->hash, label->addr);
}

void ir_section_dump(struct ir_section *section) {
	unsigned i;
	struct ir_list *iter;
	printf("Section was on line %u, prenum is %s num is %u\n", section->line, section->prenum, section->num);
	printf("Section has %u labels, %u precommands, %u commands\n", section->numlabels, section->numprecommands, section->numcommands);
	for(i = 0, iter = section->labels; i < section->numlabels; i++, iter = iter->next) {
		printf("Dumping label %u\n", i);
		ir_label_dump((struct ir_label *)iter->type);
	}
	
	for(i = 0, iter = section->precommands; i < section->numprecommands; i++, iter = iter->next) {
		printf("Dumping precommand %u\n", i);
		ir_pre_generic_dump((struct ir_pre_generic *)iter->type);
	}
	
	for(i = 0, iter = section->commands; i < section->numcommands; i++, iter = iter->next) {
		printf("Dumping command %u\n", i);
		ir_generic_dump((struct ir_generic *)iter->type);
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
	printf("%s is a stub!\n", __func__);
}

void ir_label_free(struct ir_label *label) {
	free(label->name);
	free(label);
}

void ir_section_free(struct ir_section *section) {
	unsigned i;
	struct ir_list *iter, *olditer;
	
	for(i = 0, iter = section->labels, olditer = NULL; i < section->numlabels; i++, olditer = iter, iter = iter->next) {
		free(olditer);
		ir_label_free((struct ir_label *)iter->type);
	}
	free(olditer);
	
	for(i = 0, iter = section->precommands, olditer = NULL; i < section->numprecommands; i++, olditer = iter, iter = iter->next) {
		free(olditer);
		ir_pre_generic_free((struct ir_pre_generic *)iter->type);
	}
	free(olditer);
	
	for(i = 0, iter = section->commands, olditer = NULL; i < section->numcommands; i++, olditer = iter, iter = iter->next) {
		free(olditer);
		ir_generic_free((struct ir_generic *)iter->type);
	}
	free(olditer);
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


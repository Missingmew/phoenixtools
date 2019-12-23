#ifndef PHOENIXSCRIPT_COMMANDS_H
#define PHOENIXSCRIPT_COMMANDS_H

#include <stdint.h>

/* this has to be provided and set by main */
extern unsigned int gamenum;

typedef struct cmd {
	char name[32];
	int args;
	int (*print)(char *, uint16_t *, struct cmd *);
} command;

int printCmdGeneric(char *text, uint16_t *args, command *op);
int printCmdColor(char *text, uint16_t *args, command *op);
int printCmdName(char *text, uint16_t *args, command *op);
int printCmdBackground(char *text, uint16_t *args, command *op);
int printCmdJump(char *text, uint16_t *args, command *op);
int printCmdShiftBackground(char *text, uint16_t *args, command *op);
int printCmdPerson(char *text, uint16_t *args, command *op);
int printCmdFademusic(char *text, uint16_t *args, command *op);
int printCmdChoice(char *text, uint16_t *args, command *op);
int printCmdAudio(char *text, uint16_t *args, command *op);
int printCmdAnimation(char *text, uint16_t *args, command *op);

extern command opcodeList[144];

#endif

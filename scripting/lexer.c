#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "lexer.h"
#include "common.h"
#include "commands.h"

#define SPELLLENGTH 256

#define TAKEIT {if(!lexer_take()) return 0;}
#define SKIPIT {if(!lexer_skip()) return 0;}
#define SETTOKEN(x) { currenttoken.type = x; currenttoken.string = state.string; }

char *tokentypestrings[] = { "TEXT", "SECTION", "ENDSECTION", "STRING", "IDENT", "INTEGER", "PLUS", "COLON", "COMMA", "TEXTEOF", "NUMTOKENS" };

typedef struct {
	char *input;
	char *current;
	char *string;
	unsigned int idx;
	unsigned int line;
	unsigned int column;
	unsigned int lastline;
	unsigned int lastcolumn;
} lexerstate;

token currenttoken;
lexerstate state = { NULL, NULL, 0, 0, 0, 0 };

void lexer_init(char *source) {
	state.input = source;
	state.current = state.input;
	state.string = calloc(SPELLLENGTH+1, 1);
	state.idx = 0;
	state.line = 1;
	state.column = 1;
	state.lastline = 1;
	state.lastcolumn = 1;
}

void lexer_finalize(void) {
	if(currenttoken.type != TEXTEOF) printf("lexer: error during finalize, token was not EOF\n");
	if(state.string) free(state.string);
	if(currenttoken.string) free(currenttoken.string);
}

unsigned int lexer_skip(void) {
	if(!(*(state.current))) {
		printf("lexer: Got EOF during scan, token started at line %u, column %u\n", state.lastline, state.lastcolumn);
		return 0;
	}
	if(*(state.current) == '\n') {
		state.line++;
		state.column = 1;
	}
	else state.column++;
	state.current++;
	return 1;
}

unsigned int lexer_take(void) {
	if(state.idx == SPELLLENGTH-1) {
		printf("token at line %u, column %u would exceed maximum spelling of 256 characters at line %u, column %u\n", state.lastline, state.lastcolumn, state.line, state.column);
		return 0;
	}
	state.string[state.idx] = *(state.current);
	state.idx++;
	return lexer_skip();
}

unsigned int lexer_scan(void) {
	if(!state.input) {
		printf("lexer: not initialized\n");
		return 0;
	}
	
	// skip whitespace
	while(*(state.current) == ' ' || *(state.current) == '\r' || *(state.current) =='\t' || *(state.current) =='\n') SKIPIT
	
	while((*(state.current) == '/') && (*(state.current+1) == '*')) {
		printf("lexer: comment begin at line %u column %u\n", state.line, state.column);
		SKIPIT
		SKIPIT
		while(1) {
			if((*(state.current) == '*')) {
				SKIPIT
				if((*(state.current)) == '/') {
					SKIPIT
					printf("lexer: comment end at line %u column %u\n", state.line, state.column);
					state.idx = 0;
					break;
				}
			}
			else SKIPIT
		}
		// skip follow up whitespace
		while(*(state.current) == ' ' || *(state.current) == '\r' || *(state.current) =='\t' || *(state.current) =='\n') SKIPIT
	}
	
	state.lastline = state.line;
	state.lastcolumn = state.column;
	
	if(*(state.current)) {
		// string
		if(*(state.current) == '"') {
			SKIPIT
			while(*(state.current) != '"' || *(state.current-1) == '\\') {
				TAKEIT
			}
			SKIPIT
			SETTOKEN(STRING)
			
		}
		// regular token
		else if(isalpha(*(state.current)) || *(state.current) == '.') {
			unsigned i;
			// identifier
			TAKEIT
			while(isalpha(*(state.current)) || isdigit(*(state.current)) || *(state.current) == '_') TAKEIT
			
			/* compare all command keywords here */
			for(i = 0; i < sizeofarr(commandnames); i++) {
				if(!strcmp(state.string, commandnames[i])) {
					SETTOKEN(i) // probably evil?
					break;
				}
			}
			if(i == sizeofarr(commandnames)) {
				// not a command, check other reserved keywords
				if(!strcmp(state.string, "SECTION")) SETTOKEN(SECTION)
				else if(!strcmp(state.string, "ENDSECTION")) SETTOKEN(ENDSECTION)
				else if(!strcmp(state.string, "text")) SETTOKEN(TEXT)
				else SETTOKEN(IDENT)
			}
		}
		else if(isdigit(*(state.current))) {
			// integer
			while(isdigit(*(state.current))) TAKEIT
			SETTOKEN(INTEGER)
		}
		else {
			switch(*(state.current)) {
				case '+': {
					TAKEIT
					SETTOKEN(PLUS)
					break;
				}
				case ':': {
					TAKEIT
					SETTOKEN(COLON)
					break;
				}
				case ',': {
					TAKEIT
					SETTOKEN(COMMA)
					break;
				}
				default: {
					printf("lexer: unknown character %c in line %u col %u\n", *(state.current), state.lastline, state.lastcolumn);
					return 0;
				}
			}
		}
		
		state.string = calloc(SPELLLENGTH+1, 1);
		state.idx = 0;
	}
	else {
		// EOF
		currenttoken.type = TEXTEOF;
		currenttoken.string = NULL;
		free(state.string);
		state.string = NULL;
	}
	
	currenttoken.line = state.lastline;
	currenttoken.column = state.lastcolumn;
	
	//~ printf("lexer: added token \"%s\" of type %s from line %d column %d\n", currenttoken.string ? currenttoken.string : "NULL", currenttoken.type < sizeofarr(commandnames) ? commandnames[currenttoken.type] : tokentypestrings[currenttoken.type-sizeofarr(commandnames)], currenttoken.line, currenttoken.column);
	
	return 1;
}
	
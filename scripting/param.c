#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "param.h"
#include "common.h"

void print_help(char *argv0) {
	printf("usage: %s [options]\n", argv0);
	
	printf("REQUIRED options are:\n");
	printf("-i FILE   input file\n");
	printf("-g NUM    game number (see below)\n");
	
	printf("OPTIONAL  options are\n");
	printf("-h        print this help\n");
	printf("-o FILE   output file (optional)\n");
	printf("-j        enable japanese mode\n");
	printf("-u        enable unity mode (experimental!)\n");
	printf("-fs FILE  sound name file\n");
	printf("-fp FILE  person name file\n");
	printf("-fa FILE  animation name file\n");
	printf("-fb FILE  background name file\n");
	printf("-fl FILE  location name file\n");
	printf("-fe FILE  evidence name file\n");
	
	printf("game numbers are as follows:\n");
	printf("1 - original phoenix wright\n");
	printf("2 - justice for all\n");
	printf("3 - trials and tribulations\n");
	printf("4 - apollo justice\n");
	printf("5 - Gyakuten Saiban 1 (GBA)\n");
}

void params_cleanup(struct params *par) {
	if(par->autoalloc_out) free(par->outfile);
}

unsigned parse_args(struct params *ret, int argc, char **argv, char *autosuffix) {
	#define ADVANCE { argc--; argv++; }
	#define CHECKARGC { if(argc < 1) { printf("argument %s requires an additional parameter, but there are none left!\n", cur); return 0; } }
	char *cur, *argv0 = *argv;
	memset(ret, 0, sizeof(struct params));
	ADVANCE;
	if(argc < 1) {
		print_help(argv0);
		return 0;
	}
	while(argc) {
		cur = *argv;
		ADVANCE;
		if(cur[0] != '-') {
			printf("invalid argument %s\n", cur);
			print_help(argv0);
			return 0;
		}
		switch(cur[1]) {
			case 'h': {
				print_help(argv0);
				return 0;
			}
			case 'i': {
				CHECKARGC;
				ret->infile = *argv;
				ADVANCE;
				break;
			}
			case 'g': {
				CHECKARGC;
				unsigned gamenum = strtoul(*argv, NULL, 10)-1;
				if(gamenum >= GAME_NUMGAMES) {
					printf("unsupported game number %u\n", gamenum+1);
					print_help(argv0);
					return 0;
				}
				if(gamenum == GAME_GS1GBA) ret->isjp = 1;
				ret->gamenum = gamenum;
				ADVANCE;
				break;
			}
			case 'o': {
				CHECKARGC;
				ret->outfile = *argv;
				ADVANCE;
				break;
			}
			case 'j': {
				ret->isjp = 1;
				break;
			}
			case 'u': {
				ret->isunity = 1;
				break;
			}
			case 'f': {
				CHECKARGC;
				switch(cur[2]) {
					case 's': {
						ret->soundfile = *argv;
						break;
					}
					case 'p': {
						ret->speakerfile = *argv;
						break;
					}
					case 'a': {
						ret->animfile = *argv;
						break;
					}
					case 'b': {
						ret->bgfile = *argv;
						break;
					}
					case 'l': {
						ret->locationfile = *argv;
						break;
					}
					case 'e': {
						ret->evidencefile = *argv;
						break;
					}
					default: {
						printf("unknown option %s\n", cur);
						print_help(argv0);
						return 0;
					}
				}
				ADVANCE;
				break;
			}
			default: {
				printf("unknown option %s\n", cur);
				print_help(argv0);
				return 0;
			}
		}
	}
	/* if we manage to get here we had all valid arguments
	   in case we didnt get an output filename, construct one from the input filename using autosuffix */
	if(!ret->outfile) {
		ret->outfile = malloc(strlen(ret->infile) + 1 + strlen(autosuffix) + 1);
		sprintf(ret->outfile, "%s.%s", ret->infile, autosuffix);
		ret->autoalloc_out = 1;
	}
	return 1;
}

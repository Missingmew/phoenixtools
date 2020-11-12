#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "phoenixscript_charsets.h"
#include "phoenixscript_commands.h"

char *supportedgamenames[] = {
	"PWAA", "JFA", "TT", "AJAA", "GS1GBA"
};

int compare_uint32(const void *a, const void *b) {
	uint32_t arg1 = *((uint32_t *)a);
	uint32_t arg2 = *((uint32_t *)b);
	if(arg1 < arg2) return -1;
	if(arg1 > arg2) return 1;
	return 0;
}

/* returns non-zero if token is text, zero if command */
unsigned int prepareToken(uint16_t *token, unsigned int gamenum, unsigned int isjp, unsigned int isunity) {
	if(*token < 128 || (gamenum == GAME_APOLLO && *token < 128+16)) return 0; // opcode for script
	//~ printf("original token is %08x ", *token);
	*token -= 128;
	if(gamenum == GAME_APOLLO) *token -= 16; // apollo has 16 extra opcodes
	if(isunity) *token += 32; // unity uses ASCII with offset?
	//~ printf("modified token is %08x\n", *token);
	return 1;
}

void escapeText(char *dst, char* src) {
	// escape collected text
	for(unsigned i = 0, j = 0; src[i]; i++) {
		switch(src[i]) {
			case '\n': {
				dst[j] = '\\';
				dst[j+1] = 'n';
				j+=2;
				dst[j] = 0;
				break;
			}
			case '\'': {
				dst[j] = '\\';
				dst[j+1] = '\'';
				j+=2;
				dst[j] = 0;
				break;
			}
			case '\"': {
				dst[j] = '\\';
				dst[j+1] = '\"';
				j+=2;
				dst[j] = 0;
				break;
			}
			//~ case '%': {
				//~ dst[j] = '%';
				//~ dst[j+1] = '%';
				//~ j+=2;
				//~ dst[j] = 0;
				//~ break;
			//~ }
			default: {
				dst[j] = src[i];
				j++;
				dst[j] = 0;
				break;
			}
		}
	}
}

int main( int argc, char **argv ) {
	FILE *f, *o;
	unsigned i, j;
	unsigned int fileSize, gamenum, intext, numlocaljumps;
	uint32_t numScripts, *scriptOffsets = NULL, *jumplutaddrs = NULL;
	struct localjumpinfo *localjumps = NULL;
	//~ command *curop;
	char *escapebuf = NULL;
	struct scriptstate state;
	memset(&state, 0, sizeof(state));
	if( argc < 3 ) {
		printf("Not enough args!\nUse: %s [binary script] [gamenum]\nwhere gamenum is\n", argv[0]);
		printf("1 - original phoenix wright\n2 - justice for all\n3 - trials and tribulations\n4 - apollo justice\n5 - Gyakuten Saiban 1 (GBA)\n\n");
		printf("add 10 to enable compat for japanese in non-unity versions\nadd 20 to enable unity mode\nGBA versions imply japanese mode, do not explicitly enable it\n");
		return 1;
	}
	
	gamenum = strtoul(argv[2], NULL, 10) - 1;
	
	//~ printf("gamenum is %u %u\n", gamenum, gamenum == GAME_GS1GBA);
	if(gamenum == GAME_GS1GBA) {
		state.isjp = 1;
	}
	else {
		if(gamenum > 19) {
			state.isunity = 1;
			gamenum -= 20;
		}
		else if(gamenum > 9) {
			state.isjp = 1;
			gamenum -= 10;
		}
	}
	if( gamenum >= GAME_NUMGAMES ) {
		printf("unsupported gamenum %d\n", gamenum+1);
		return 1;
	}
	
	state.gamenum = gamenum;
	
	/* ensure sane combo of gamenum, isunity and isjp */
	if(state.gamenum == GAME_APOLLO && state.isunity) {
		printf("apollo does not have a unity version\n");
		return 1;
	}
	if(state.isjp && state.isunity) {
		printf("there is no explicit japanese support for unity right now\n");
		return 1;
	}
	
	printf("selected config: %s %s %s\n", supportedgamenames[state.gamenum], state.isjp ? "jp" : "", state.isunity ? "unity" : "");
	
	if( !(f = fopen( argv[1], "rb" ))) {
		printf("Couldnt open file %s\n", argv[1]);
		return 1;
	}
	
	char *outfilename = malloc( strlen( argv[1] ) + 4 + 1 );
	
	sprintf( outfilename, "%s.txt", argv[1] );
	if( !(o = fopen( outfilename, "w" ))) {
			printf("Couldnt open file %s\n", outfilename);
			return 1;
	}
	
	fseek( f, 0, SEEK_END );
	fileSize = ftell(f);
	fseek( f, 0, SEEK_SET );
	fread( &numScripts, sizeof(uint32_t), 1, f );
	scriptOffsets = malloc( numScripts * sizeof(uint32_t));
	/* need this to find number of sections and specialdata */
	jumplutaddrs = malloc( numScripts * sizeof(uint32_t));
	
	fread(scriptOffsets, sizeof(uint32_t)*numScripts, 1, f);
	
	state.scriptsize = fileSize - scriptOffsets[0];
	state.textidx = 0;
	state.scriptidx = 0;
	state.textfile = malloc(0x40000); // 256k
	state.maxtext = 0x40000; // 256k
	state.script = malloc(state.scriptsize);
	
	state.outidx = 0;
	state.outbuf = malloc(128);
	state.maxoutbuf = 128;
	state.outbuf[0] = 0;
	escapebuf = malloc(128*2); // escapebuf always maxoutbuf*2
	
	state.section = 0;
	state.sectionoff = 0;
	state.sectionlist = scriptOffsets;
	state.jumplut = NULL;
	state.numjumplut = 0;
	state.numsections = 0;
	
	/* disable text output to scan for commands that use special data */
	state.outputenabled = 0;
	
	fread(state.script, state.scriptsize, 1, f);
	
	/* find the beginning and amount of "special" data */
	/* parse the script, catching all cmd35 and cmd36/78 (which are known to use special data)
	   and saving the indices of the "scriptOffsets" they access */
	for(i = 0, j = 0, numlocaljumps = 0; i < state.scriptsize/2; i++) {
		if((state.gamenum == GAME_APOLLO && state.script[i] > 0x8F) || (state.gamenum != GAME_APOLLO && state.script[i] > 0x7F)) continue;
		switch(state.script[i]) {
			case 0x35: {
				if(state.script[i+1] & 0x80 && state.script[i+2] && numScripts > state.script[i+2]) {
					jumplutaddrs[j++] = state.script[i+2];
				}
				else if(!(state.script[i+1] & 0x80)) {
					localjumps = realloc(localjumps, (numlocaljumps+1)*sizeof(struct localjumpinfo));
					localjumps[numlocaljumps].index = i;
					localjumps[numlocaljumps++].target = state.script[i+2]/2;
				}
				i += 2;
				break;
			}
			/* 78 is mapped to 36 in unity... 7A uses the same principle for our purposes */
			case 0x36:
			case 0x78:
			case 0x7A: {
				if(state.script[i+1] && numScripts > state.script[i+1]) {
					jumplutaddrs[j++] = state.script[i+1];
				}
				i += 1;
				break;
			}
			default: {
				i += printcommands[state.script[i]](&state);
				break;
			}
		}
	}
	if(j) {
		/* if we found any uses of specialdata, find the first used index and use that
		   to mark the end of actual section offsets and the beginning of specialdata
		   as well as calculate the number of each */
		
		/* jumplutaddrs now holds all known indices for j uses of special data
		   qsort the array and use the first index from the result as begin of specialdata */
		qsort(jumplutaddrs, j, sizeof(uint32_t), compare_uint32);
		
		//~ printf("found the following specialaddr indices:\n");
		//~ for(unsigned i = 0; i < j; i++) printf("%08x\n", jumplutaddrs[i]);
		
		//~ printf("first specialdata index is %08x(%08x)\n", jumplutaddrs[0], jumplutaddrs[0]*4+4);
		//~ printf("have %08x special uint32\n", (numScripts-jumplutaddrs[0]));
		
		state.jumplut = (jumplutpack *)&scriptOffsets[jumplutaddrs[0]];
		state.numjumplut = (numScripts-jumplutaddrs[0]);
		state.numsections = jumplutaddrs[0];
		//~ fprintf(stderr, "start of specialdata at %08x\n", 4+jumplutaddrs[0]*4);
		//~ printf("have %u specials\n", state.numjumplut);
	}
	else {
		state.numsections = numScripts;
	}
	
	state.numlocaljumps = numlocaljumps;
	state.localjumps = localjumps;
	
	/* fix up sectionlist to have it hold the indices used during the dump */
	for(i = 1; i < state.numsections; i++) state.sectionlist[i] = (state.sectionlist[i]-state.sectionlist[0])/2;
	state.sectionlist[0] = 0;
	/* fix up local jump structs to make index hold the appropriate section */
	for(i = 0; i < state.numlocaljumps; i++) {
		for(j = 1; j < state.numsections; j++) {
			if(state.localjumps[i].index < state.sectionlist[j]) {
				state.localjumps[i].index = j-1;
				break;
			}
		}
		if(j == state.numsections) state.localjumps[i].index = j-1;
	}
	
	//~ printf("have %u specialdata and %u(%08x) sections\n", state.numjumplut, state.numsections, state.numsections);
	
	/* enable text output for actual dumping */
	state.outputenabled = 1;
	
	//~ numScripts--;
	//~ state.textidx += sprintf( state.textfile+state.textidx, "begin special data\n");
	//~ for(unsigned i = 0; i < state.numjumplut; i++) state.textidx +=sprintf( state.textfile+state.textidx, "%04x %04x\n", state.jumplut[i].val0, state.jumplut[i].val1);
	//~ state.textidx += sprintf( state.textfile+state.textidx, "end special data\n");
	
	state.textidx += sprintf(state.textfile+state.textidx, "SECTION 0\n" );
	intext = 0;
	while(state.scriptidx < state.scriptsize/2) {
		if(state.maxtext - 1024 < state.textidx) {
			printf("converted textfile is approaching current limit of 0x%x bytes, reallocing\n", state.maxtext);
			if(!(state.textfile = realloc(state.textfile, state.maxtext*2))) {
				printf("couldnt realloc\n");
				return 1;
			}
			state.maxtext *= 2;
		}
		if(state.maxoutbuf - 10 < state.outidx) {
			printf("internal buffer for converting text approaching current limit of 0x%x bytes, reallocing\n", state.maxoutbuf);
			if(!(state.outbuf = realloc(state.outbuf, state.maxoutbuf*2))) {
				printf("couldnt realloc internal out\n");
				return 1;
			}
			if(!(escapebuf = realloc(escapebuf, state.maxoutbuf*2*2))) {
				printf("couldnt realloc internal out\n");
				return 1;
			}
			state.maxoutbuf *= 2;
		}
		if(isSectionStart(state.sectionlist, state.numsections, state.scriptidx) > 0) {
			state.section = isSectionStart(state.sectionlist, state.numsections, state.scriptidx);
			state.sectionoff = state.scriptidx;
			state.textidx += sprintf(state.textfile+state.textidx, "ENDSECTION\nSECTION %u\n", state.section);
		}
		if(isLabelLocation(state.jumplut, state.numjumplut, state.section, state.scriptidx - state.sectionoff) > -1) {
			state.textidx += sprintf(state.textfile+state.textidx, "label%u_%u:\n", state.section, state.scriptidx - state.sectionoff);
		}
		else if(isLocalLabelLocation(state.localjumps, state.numlocaljumps, state.section, state.scriptidx - state.sectionoff) > -1) {
			state.textidx += sprintf(state.textfile+state.textidx, ".label%u_%u:\n", state.section, state.scriptidx - state.sectionoff);
		}
		//~ printf("memidx %08x (off %08x)\n", state.scriptidx, state.scriptidx*2+numScripts*4);
		if(prepareToken(&state.script[state.scriptidx], state.gamenum, state.isjp, state.isunity)) {
			if(!intext) {
				state.textstart = state.scriptidx;
				intext = 1;
			}
			if(state.isunity) {
				// this is incredibly evil on a second thought... should probably find a better solution
				state.outidx += sprintf( state.outbuf+state.outidx, "%c", (char)state.script[state.scriptidx]);
			}
			else {
				switch(charset_isTokenValid(state.script[state.scriptidx], state.isjp, state.gamenum)) {
					case SET_SHARED: {
						state.outidx += sprintf(state.outbuf+state.outidx, "%s", charset_shared[state.script[state.scriptidx]]);
						break;
					}
					case SET_DEFAULT: {
						state.outidx += sprintf(state.outbuf+state.outidx, "%s", charset_default[state.isjp][state.script[state.scriptidx]]);
						break;
					}
					case SET_EXTENDED: {
						state.outidx += sprintf(state.outbuf+state.outidx, "%s", charset_japanese_extended[ARRGAMENUM(state.gamenum)][state.script[state.scriptidx]-256]);
						break;
					}
					default: {
						/* absolutely evil hacks: the english scripts in phoenix 2 from the en,fr,de version do not appear to fully use the "normal" euro charset */
						if(state.gamenum == GAME_PHOENIX2) {
							if(state.script[state.scriptidx]+128 == 1545) {
								state.outidx += sprintf(state.outbuf+state.outidx, "%s", charset_shared[0x16A]); // maps to ”
								break;
							}
							else if(state.script[state.scriptidx]+128 == 1548) {
								state.outidx += sprintf(state.outbuf+state.outidx, "%s", charset_default[state.isjp][0xCE]); // maps to $
								break;
							}
							else if(state.script[state.scriptidx]+128 == 1554) {
								state.outidx += sprintf(state.outbuf+state.outidx, "%s", charset_default[state.isjp][0x154]); // maps to é
								break;
							}
							else if(state.script[state.scriptidx]+128 == 1556) {
								state.outidx += sprintf(state.outbuf+state.outidx, "%s", charset_default[state.isjp][0xDE]); // maps to ;
								break;
							}
							else if(state.script[state.scriptidx]+128 == 1557) {
								state.outidx += sprintf(state.outbuf+state.outidx, "%s", charset_default[state.isjp][0xDA]); // maps to ç
								break;
							}
							else if(state.script[state.scriptidx]+128 == 1558) {
								state.outidx += sprintf(state.outbuf+state.outidx, "%s", charset_default[state.isjp][0xCF]); // maps to à
								break;
							}
						}
						state.outidx += sprintf(state.outbuf+state.outidx, "{%05u}", state.script[state.scriptidx]+128);
						break;
					}
				}
			}
			state.scriptidx++;
		}
		else {
			// do indentation
			if(!intext) state.textidx += sprintf(state.textfile+state.textidx, " %04u\t", state.scriptidx - state.sectionoff);
			if(state.outidx && state.script[state.scriptidx] != 0x01) {
				// escape collected text
				escapeText(escapebuf, state.outbuf);
				// print collected text and do indentation for command
				state.textidx += sprintf(state.textfile+state.textidx, " %04u\ttext \"%s\"\n %04u\t", state.textstart-state.sectionoff, escapebuf, state.scriptidx - state.sectionoff);
				state.outidx = 0;
				state.outbuf[0] = 0;
				intext = 0;
			}
			printcommands[state.script[state.scriptidx]](&state);
		}
	}
	// get leftover text printed
	if(state.outidx) {
		state.textidx += sprintf(state.textfile+state.textidx, "text \"%s\"\n", state.outbuf);
	}
	state.textidx += sprintf( state.textfile+state.textidx, "ENDSECTION\n" );
	fwrite(state.textfile, state.textidx, 1, o);
	fclose(f);
	fclose(o);
	free(outfilename);
	free(scriptOffsets);
	free(jumplutaddrs);
	free(state.localjumps);
	free(state.script);
	free(state.textfile);
	free(state.outbuf);
	free(escapebuf);
	return 0;
}
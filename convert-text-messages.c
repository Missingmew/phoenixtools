#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "phoenixscript_charsets.h"
#include "phoenixscript_commands.h"

#define sizeofarr(a) (sizeof(a) / sizeof(a[0]))

#define OUTBUFSIZE 256

char *supportedgames[] = {
	"PWAA", "JFA", "TT", "AJAA"
};

int compare_uint32(const void *a, const void *b) {
	uint32_t arg1 = *((uint32_t *)a);
	uint32_t arg2 = *((uint32_t *)b);
	if(arg1 < arg2) return -1;
	if(arg1 > arg2) return 1;
	return 0;
}

int getOffsetIndex( unsigned int offset, uint32_t *list, unsigned int count ) {
	int i;
	for( i = 0; i < count; i++ ) if( offset == list[i] ) return i;
	return -1;
}

int getMemidxIndex( unsigned int memidx, uint32_t *list, unsigned int count ) {
	int i, sub = list[0];
	for( i = 0; i < count; i++ ) if( memidx == (list[i]-sub)/2 ) return i;
	return -1;
}

/* returns non-zero if token is text, zero if command */
unsigned int prepareToken(uint16_t *token, unsigned int gamenum, unsigned int isjp, unsigned int isunity) {
	if(*token < 128 || (gamenum == 3 && *token < 128+16)) return 0; // opcode for script
	//~ printf("original token is %08x ", *token);
	*token -= 128;
	if(gamenum == 3) *token -= 16; // apollo has 16 extra opcodes
	if(isunity) *token -= 32; // unity uses ASCII with offset?
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
	unsigned int fileSize, isunity = 0, isjp = 0, gamenum, intext;
	uint32_t numScripts, *scriptOffsets = NULL, *specialaddrs = NULL;
	//~ command *curop;
	char escapebuf[OUTBUFSIZE*2];
	struct scriptstate state;
	if( argc < 3 ) {
	printf("Not enough args!\nUse: %s [binary script] [gamenum]\nwhere gamenum is\n1 - original phoenix wright\n2 - justice for all\n3 - trials and tribulations\n4 - apollo justice\n\nadd 10 to enable compat for japanese in non-unity versions\nadd 20 to enable unity mode\n", argv[0]);
		return 1;
	}
	
	gamenum = strtoul(argv[2], NULL, 10) - 1;
	if(gamenum > 19) {
		isunity = 1;
		gamenum -= 20;
	}
	else if(gamenum > 9) {
		isjp = 1;
		gamenum -= 10;
	}
	if( gamenum > 3 ) {
		printf("unsupported gamenum %d\n", gamenum+1);
		return 1;
	}
	
	/* ensure sane combo of gamenum, isunity and isjp */
	if(gamenum == 3 && isunity) {
		printf("apollo does not have a unity version\n");
		return 1;
	}
	if(isjp && isunity) {
		printf("there is no explicit japanese support for unity right now\n");
		return 1;
	}
	
	printf("selected config: %s %s %s\n", supportedgames[gamenum], isjp ? "jp" : "", isunity ? "unity" : "");
	
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
	specialaddrs = malloc( numScripts * sizeof(uint32_t));
	
	fread(scriptOffsets, sizeof(uint32_t)*numScripts, 1, f);
	
	state.scriptsize = fileSize - scriptOffsets[0];
	state.textidx = 0;
	state.scriptidx = 0;
	state.textfile = malloc(0x40000); // 256k
	state.maxtext = 0x40000; // 256k
	state.script = malloc(state.scriptsize);
	state.outidx = 0;
	state.outbuf = malloc(OUTBUFSIZE);
	state.outbuf[0] = 0;
	state.section = 0;
	state.sectionoff = 0;
	state.sectionlist = scriptOffsets;
	state.gamenum = gamenum;
	state.isjp = isjp;
	state.specialdata = NULL;
	state.numspecialdata = 0;
	state.numsections = 0;
	
	/* disable text output to scan for commands that use special data */
	state.outputenabled = 0;
	
	fread(state.script, state.scriptsize, 1, f);
	
	/* find the beginning and amount of "special" data */
	/* parse the script, catching all cmd35 and cmd36 (which are known to use special data)
	   and saving the indices of the "scriptOffsets" they access */
	for(i = 0, j = 0; i < state.scriptsize/2; i++) {
		if(state.script[i] > 0x7F || (gamenum == 3 && state.script[i] > 0x8F)) continue;
		switch(state.script[i]) {
			case 0x35: {
				if(state.script[i+1] & 0x80 && state.script[i+2] && numScripts > state.script[i+2]) {
					specialaddrs[j++] = state.script[i+2];
				}
				i += 2;
				break;
			}
			case 0x36: {
				if(state.script[i+1] && numScripts > state.script[i+1]) {
					specialaddrs[j++] = state.script[i+1];
				}
				i += 2;
				break;
			}
			default: {
				i += commands[state.script[i]].print(&state);
				break;
			}
		}
	}
	if(j) {
		/* if we found any uses of specialdata, find the first used index and use that
		   to mark the end of actual section offsets and the beginning of specialdata
		   as well as calculate the number of each */
		
		/* specialaddrs now holds all known indices for j uses of special data
		   qsort the array and use the first index from the result as begin of specialdata */
		qsort(specialaddrs, j, sizeof(uint32_t), compare_uint32);
		
		//~ printf("found the following specialaddr indices:\n");
		//~ for(unsigned i = 0; i < j; i++) printf("%08x\n", specialaddrs[i]);
		
		//~ printf("first specialdata index is %08x\n", specialaddrs[0]);
		//~ printf("have %08x special uint32\n", (numScripts-specialaddrs[0]));
		
		state.specialdata = (uint16_t *)&scriptOffsets[specialaddrs[0]];
		state.numspecialdata = (numScripts-specialaddrs[0]) * 2;
		state.numsections = specialaddrs[0];
	}
	else {
		state.numsections = numScripts;
	}
	
	printf("have %u specialdata and %u sections\n", state.numspecialdata, state.numsections);
	
	/* enable text output for actual dumping */
	state.outputenabled = 1;
	
	//~ numScripts--;
	state.textidx += sprintf( state.textfile+state.textidx, "begin special data\n");
	for(unsigned i = 0; i < state.numspecialdata; i+=2) state.textidx +=sprintf( state.textfile+state.textidx, "%04x %04x\n", state.specialdata[i], state.specialdata[i+1]);
	state.textidx += sprintf( state.textfile+state.textidx, "end special data\n");
	//~ printf("special data %04x, %04x\n", scriptOffsets[numScripts] & 0xFFFF, scriptOffsets[numScripts] >> 16);
	
	state.textidx += sprintf(state.textfile+state.textidx, "section 0\n" );
	intext = 0;
	while( state.scriptidx < state.scriptsize/2) {
		if(state.maxtext - 100 < state.textidx) {
			printf("converted textfile is approaching current limit of 0x%x bytes, reallocing\n", state.maxtext);
			if(!(state.textfile = realloc(state.textfile, state.maxtext*2))) {
				printf("couldnt realloc\n");
				return 1;
			}
			state.maxtext *= 2;
		}
		if( getMemidxIndex( state.scriptidx, state.sectionlist, state.numsections ) > 0 ) {
			state.section = getMemidxIndex( state.scriptidx, state.sectionlist, state.numsections );
			state.sectionoff = state.scriptidx;
			state.textidx += sprintf( state.textfile+state.textidx, "endsection\nsection %03u\n", state.section);
		}
		//~ printf("memidx %08x (off %08x)\n", state.scriptidx, state.scriptidx*2+numScripts*4);
		if(prepareToken(&state.script[state.scriptidx], gamenum, isjp, isunity)) {
			if(!intext) {
				state.textstart = state.scriptidx;
				intext = 1;
			}
			if(isunity) {
				// this is incredibly evil on a second thought... should probably find a better solution
				state.outidx += sprintf( state.outbuf+state.outidx, "%c", (char)state.script[state.scriptidx]);
			}
			else {
				switch(charset_isTokenValid(state.script[state.scriptidx], isjp, gamenum)) {
					case SET_SHARED: {
						state.outidx += sprintf(state.outbuf+state.outidx, "%s", charset_shared[state.script[state.scriptidx]]);
						break;
					}
					case SET_DEFAULT: {
						state.outidx += sprintf(state.outbuf+state.outidx, "%s", charset_default[isjp][state.script[state.scriptidx]]);
						break;
					}
					case SET_EXTENDED: {
						state.outidx += sprintf(state.outbuf+state.outidx, "%s", charset_japanese_extended[gamenum][state.script[state.scriptidx]-256]);
						break;
					}
					default: {
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
			commands[state.script[state.scriptidx]].print(&state);
		}
	}
	// get leftover text printed
	if(state.outidx) {
		state.textidx += sprintf(state.textfile+state.textidx, "text \"%s\"\n", state.outbuf);
	}
	state.textidx += sprintf( state.textfile+state.textidx, "endsection\n" );
	fwrite(state.textfile, state.textidx, 1, o);
	fclose(f);
	fclose(o);
	free(outfilename);
	free(scriptOffsets);
	free(specialaddrs);
	free(state.script);
	free(state.textfile);
	free(state.outbuf);
	return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "phoenixscript_charsets.h"
#include "phoenixscript_commands.h"

#define sizeofarr(a) (sizeof(a) / sizeof(a[0]))

char *supportedgames[] = {
	"PWAA", "JFA", "TT", "AJAA"
};

int getOffsetIndex( unsigned int offset, uint32_t *list, unsigned int count ) {
	int i;
	for( i = 0; i < count; i++ ) if( offset == list[i] ) return i;
	return -1;
}

int getMemidxIndex( unsigned int memidx, uint32_t *list, unsigned int count ) {
	int i, sub = (count*4)+4;
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

int main( int argc, char **argv ) {
	FILE *f, *o;
	unsigned int fileSize, isunity = 0, isjp = 0, gamenum;
	uint32_t numScripts, *scriptOffsets = NULL;
	//~ command *curop;
	char escapebuf[512];
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
	fread(scriptOffsets, sizeof(uint32_t)*numScripts, 1, f);
	
	state.scriptsize = fileSize - ((numScripts*4)+4);
	state.textidx = 0;
	state.scriptidx = 0;
	state.textfile = malloc(0x40000); // 256k
	state.maxtext = 0x40000; // 256k
	state.script = malloc(state.scriptsize);
	state.outidx = 0;
	state.outbuf = malloc(256);
	state.outbuf[0] = 0;
	state.gamenum = gamenum;
	
	fread(state.script, state.scriptsize, 1, f);
	
	state.textidx += sprintf(state.textfile, "section 0\n" );
	while( state.scriptidx < state.scriptsize/2 - 2 ) {
		if(state.maxtext - 100 < state.textidx) {
			printf("converted textfile is approaching current limit of 0x%x bytes, reallocing\n", state.maxtext);
			if(!(state.textfile = realloc(state.textfile, state.maxtext*2))) {
				printf("couldnt realloc\n");
				return 1;
			}
			state.maxtext *= 2;
		}
		if( getMemidxIndex( state.scriptidx, scriptOffsets, numScripts ) > 0 ) {
			state.textidx += sprintf( state.textfile+state.textidx, "endsection\nsection %03u\n", getMemidxIndex( state.scriptidx, scriptOffsets, numScripts ));
		}
		//~ printf("memidx %08x (off %08x)\n", state.scriptidx, state.scriptidx*2+numScripts*4);
		if(prepareToken(&state.script[state.scriptidx], gamenum, isjp, isunity)) {
			if(isunity) {
				// this is incredibly evil on a second thought... should probably find a better solution
				state.outidx += sprintf( state.outbuf+state.outidx, "%c", (char)state.script[state.scriptidx]);
			}
			else {
				if(state.script[state.scriptidx] < sizeofarr(charset_shared)) { /* token is within default charsets */
					if(charset_shared[state.script[state.scriptidx]] != 0) { /* char is in shared charset */
						state.outidx += sprintf( state.outbuf+state.outidx, "%s", charset_shared[state.script[state.scriptidx]]);
					}
					else if(charset_default[isjp][state.script[state.scriptidx]] != 0) { /* char is in default charset for selected localization */
						state.outidx += sprintf( state.outbuf+state.outidx, "%s", charset_default[isjp][state.script[state.scriptidx]]);
					}
				}
				else if(isjp && (state.script[state.scriptidx]-256 < sizeofarr(charset_japanese_extended[gamenum]))) { /* token is within extended charset of game */
					state.script[state.scriptidx] -= 256;
					if(charset_japanese_extended[gamenum][state.script[state.scriptidx]] != 0) {
						state.outidx += sprintf( state.outbuf+state.outidx, "%s", charset_japanese_extended[gamenum][state.script[state.scriptidx]]);
					}
					else { /* char is not in any charset */
						state.outidx += sprintf( state.outbuf+state.outidx, "{%05u}", state.script[state.scriptidx]+128+256 );
					}
				}
				else { /* char is not in any charset */
					state.outidx += sprintf( state.outbuf+state.outidx, "{%05u}", state.script[state.scriptidx]+128 );
				}
			}
			state.scriptidx++;
		}
		else {
			// do indentation
			state.textidx += sprintf(state.textfile+state.textidx, "\t");
			if(state.outidx && state.script[state.scriptidx] != 0x01) {
				// escape collected text
				for(unsigned i = 0, j = 0; state.outbuf[i]; i++) {
					switch(state.outbuf[i]) {
						case '\n': {
							escapebuf[j] = '\\';
							escapebuf[j+1] = 'n';
							j+=2;
							escapebuf[j] = 0;
							break;
						}
						case '\'': {
							escapebuf[j] = '\\';
							escapebuf[j+1] = '\'';
							j+=2;
							escapebuf[j] = 0;
							break;
						}
						case '\"': {
							escapebuf[j] = '\\';
							escapebuf[j+1] = '\"';
							j+=2;
							escapebuf[j] = 0;
							break;
						}
						//~ case '%': {
							//~ escapebuf[j] = '%';
							//~ escapebuf[j+1] = '%';
							//~ j+=2;
							//~ escapebuf[j] = 0;
							//~ break;
						//~ }
						default: {
							escapebuf[j] = state.outbuf[i];
							j++;
							escapebuf[j] = 0;
							break;
						}
					}
				}
				// print collected text and do indentation for command
				state.textidx += sprintf(state.textfile+state.textidx, "text \"%s\"\n\t", escapebuf);
				state.outidx = 0;
				state.outbuf[0] = 0;
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
	free(state.script);
	free(state.textfile);
	free(state.outbuf);
	return 0;
}
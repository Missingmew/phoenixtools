#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "phoenixscript_charsets.h"
#include "phoenixscript_commands.h"

#define sizeofarr(a) (sizeof(a) / sizeof(a[0]))

/* need to provide this for commands*/
unsigned int gamenum;

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
	unsigned int fileSize, i, memidx, textidx = 0, tbufidx = 0, scriptsize, isunity = 0, isjp = 0;
	uint32_t numScripts, *scriptOffsets = NULL;
	uint16_t token, arguments[10] = {0}, *scriptfile = NULL;
	command *curop;
	char textfile[0x100000] = {0}; /* 1M should be enough */
	char textbuf[256] = {0};
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
	scriptsize = fileSize - ((numScripts*4)+4);
	scriptfile = malloc(scriptsize);
	fread(scriptfile, scriptsize, 1, f);
	memidx = 0;
	
	textidx += sprintf(textfile, "section 0" );
	while( memidx < scriptsize/2 - 2 ) {
		if(textidx > (0x100000-100)) {
			printf("converted textfile is approaching the 1M limit, now at 100 or less chars remaining, aborting\n");
			return 1;
		}
		if( getMemidxIndex( memidx, scriptOffsets, numScripts ) > 0 ) {
			textidx += sprintf( textfile+textidx, "endsection\nsection %03u\n", getMemidxIndex( memidx, scriptOffsets, numScripts));
		}
		// printf("memidx %08x (off %08x)\n", memidx, memidx*2+numScripts*4);
		token = scriptfile[memidx];
		memidx++;
		if(prepareToken(&token, gamenum, isjp, isunity)) {
			if(isunity) {
				tbufidx += sprintf( textbuf+tbufidx, "%c", (char)token);
			}
			else {
				if(token < sizeofarr(charset_shared)) { /* token is within default charsets */
					if(charset_shared[token] != 0) { /* char is in shared charset */
						tbufidx += sprintf( textbuf+tbufidx, "%s", charset_shared[token]);
					}
					else if(charset_default[isjp][token] != 0) { /* char is in default charset for selected localization */
						tbufidx += sprintf( textbuf+tbufidx, "%s", charset_default[isjp][token]);
					}
				}
				else if(isjp && (token-256 < sizeofarr(charset_japanese_extended[gamenum]))) { /* token is within extended charset of game */
					token -= 256;
					if(charset_japanese_extended[gamenum][token] != 0) {
						tbufidx += sprintf( textbuf+tbufidx, "%s", charset_japanese_extended[gamenum][token]);
					}
					else { /* char is not in any charset */
						tbufidx += sprintf( textbuf+tbufidx, "{%05u}", token+128+256 );
					}
				}
				else { /* char is not in any charset */
					tbufidx += sprintf( textbuf+tbufidx, "{%05u}", token+128 );
				}
			}
		}
		else {
			// do indentation
			textidx += sprintf(textfile+textidx, "\t");
			if(tbufidx) {
				textidx += sprintf(textfile+textidx, "text \"%s\"\n", textbuf);
				tbufidx = 0;
				textbuf[0] = 0;
				// do indentation for command if we had text
				textidx += sprintf(textfile+textidx, "\t");
			}
			// switch to using per opcode functions
			curop = &opcodeList[token];
			if(token > 0x7F) printf("apollo - curtoken %08x\n", token);
			if( curop->args > 0 ) {
				for( i = 0; i < curop->args; i++ ) {
					arguments[i] = scriptfile[memidx];
					memidx++;
				}
			}
			
			textidx += curop->print(textfile+textidx, arguments, curop);
		}
	}
	// get leftover text printed
	if(tbufidx) {
		textidx += sprintf(textfile+textidx, "text \"%s\"\n", textbuf);
	}
	textidx += sprintf( textfile+textidx, "endsection\n" );
	fwrite(textfile, textidx, 1, o);
	fclose(f);
	fclose(o);
	free(outfilename);
	free(scriptOffsets);
	free(scriptfile);
	return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "ntrcom/nitrocompression.h"

typedef struct {
	uint32_t offset;
	uint32_t size;
}__attribute__((packed)) pacEntry;

typedef enum {
	LINEUSE,
	LINEEMPTY
} linetype;

int main( int argc, char** argv ) {
	
	if ( argc < 3 )
	{
		printf( "Not enough arguments given!\nUsage: %s [file] [enable decompressor] [list]\nFiles will be extracted in current working directory.\n", argv[0] );
		return 1;
	}
	
	uint32_t i, numFiles;
	unsigned int resultsize, compressedsize, decompress;
	char outputname[256], mapline[256];
	unsigned char *workbuffer = NULL, *resultbuffer = NULL;
	pacEntry *entrylist = NULL;
	linetype curline;
	FILE *f, *o, *t = NULL;
	
	decompress = strtoul(argv[2], NULL, 10);
	
	if( !(f = fopen( argv[1], "rb" ))) {
		printf("Couldnt open file %s\n", argv[1]);
		return 1;
	}
	if(argc == 4) {
		if( !(t = fopen( argv[3], "r" ))) {
			printf("Couldnt open file %s\n", argv[3]);
			return 1;
		}
	}
	
	fread( &numFiles, 4, 1, f );
	entrylist = malloc(sizeof(pacEntry) * numFiles);
	fread(entrylist, numFiles * sizeof(pacEntry), 1, f);
	
	curline = LINEEMPTY;
	resultsize = 0;
	for( i = 0; i < numFiles; i++ ) {
		printf("Current file offset: %08x - Current file size %08x\n", entrylist[i].offset, entrylist[i].size);
		if(t) {
			fgets(mapline, 256, t);
			while(!(strncmp(mapline, "//", 2)) && !feof(t)) fgets(mapline, 256, t);
			if(feof(t)) curline = LINEEMPTY;
			else if(strlen(mapline) < 3) curline = LINEEMPTY;
			else curline = LINEUSE;
		}
		
		while(strcspn(mapline, "\r\n") < strlen(mapline)) mapline[strcspn(mapline,"\r\n")] = 0;
		fseek( f, entrylist[i].offset, SEEK_SET );
		workbuffer = malloc(entrylist[i].size);
		fread( workbuffer, entrylist[i].size, 1, f );
		
		if(decompress) {
			compressedsize = entrylist[i].size;
			resultbuffer = unpackBuffer( workbuffer, &resultsize, &compressedsize );
			if(!resultbuffer) {
				printf("file %08x is uncompressed!\n", i);
				resultsize = 0;
			}
		}
		
		if(curline == LINEUSE) sprintf(outputname, "%.255s", mapline);
		else sprintf( outputname, "%04d-%04x.bin", i, i );
		
		if( !(o = fopen( outputname, "wb" ))) {
			printf("Couldnt open file %s\n", outputname);
			return 1;
		}
		if(!resultsize) fwrite( workbuffer, entrylist[i].size, 1, o );
		else fwrite( resultbuffer, resultsize, 1, o );
		fclose(o);
		
		free( workbuffer );
		if(resultsize) free( resultbuffer );
	}
	free(entrylist);
	if(t) fclose(t);
	fclose(f);
	printf("Done.\n");
	return 0;
	
}

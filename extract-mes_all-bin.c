#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "ntrcom/nitrocompression.h"

typedef struct{
	uint32_t offset;
	uint32_t size;
}__attribute__((packed)) fileInfo;

int main( int argc, char **argv ) {
	FILE *f, *o;
	unsigned int i, indexOffset, resultsize, compressedsize;
	uint32_t numScripts;
	char fileName[32];
	fileInfo script;
	unsigned char *workbuffer, *resultbuffer;
	if( argc < 2 ) {
		printf("Not enough args!\nUse: %s mes_all.bin\n", argv[0]);
		return 1;
	}
	
	if( !(f = fopen( argv[1], "rb" ))) {
		printf("Couldnt open file %s\n", argv[1]);
		return 1;
	}
	
	fread( &numScripts, sizeof(uint32_t), 1, f );
	indexOffset = ftell(f);
	for( i = 0; i < numScripts; i++ ) {
		fread( &script, sizeof(script), 1, f );
		indexOffset = ftell(f);
		sprintf( fileName, "script-%08d.bin", i );
		fseek( f, script.offset, SEEK_SET );
		workbuffer = malloc( script.size );
		fread( workbuffer, script.size, 1, f );
		compressedsize = script.size;
		resultbuffer = unpackBuffer( workbuffer, &resultsize, &compressedsize );
		printf("script %03d offset %08x(%08x) compressed %08x(%08x) uncompressed %08x(%08x)\n", i, script.offset, script.offset/4, script.size, script.size/4, resultsize, resultsize/4);
		if( !(o = fopen( fileName, "wb" ))) { 
			printf("Couldnt open file %s\n", fileName);
			return 1;
		}
		fwrite( resultbuffer, resultsize, 1, o );
		fclose(o);
		free( workbuffer );
		free( resultbuffer );
		fseek( f, indexOffset, SEEK_SET );
	}
	fclose(f);
	printf("Done.\n");
	return 0;
}
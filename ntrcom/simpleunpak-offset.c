#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "nitrocompression.h"

int main( int argc, char **argv ) {
	FILE *f, *o;
	unsigned int offset, filesize, resultsize, compressedsize;
	uint32_t tempsize = 0;
	char fileName[512];
	unsigned char *workbuffer, *resultbuffer;
	if( argc < 3 ) {
		printf("Not enough args!\nUse: %s file offset\n", argv[0]);
		return 1;
	}
	offset = strtoul(argv[2], NULL, 16);
	if( !(f = fopen( argv[1], "rb" ))) {
		printf("Couldnt open file %s\n", argv[1]);
		return 1;
	}
	
	fseek( f, 0, SEEK_END );
	filesize = ftell(f);
	fseek( f, offset+1, SEEK_SET );
	
	fread( &tempsize, 3, 1, f );
	if((offset + tempsize) > filesize) tempsize = filesize - offset;
	fseek( f, offset, SEEK_SET );
				
	sprintf( fileName, "%s-%08d-%08x.unpak", argv[1], offset, offset );
	workbuffer = malloc( tempsize );
	fread( workbuffer, tempsize, 1, f );
	resultbuffer = unpackBuffer( workbuffer, &resultsize, &compressedsize );
	if( !resultbuffer ) {
		printf("couldnt unpak file\n");
		free( workbuffer );
		fclose(f);
		return 1;
	}
	if( !(o = fopen( fileName, "wb" ))) {
		printf("Couldnt open file %s\n", fileName);
		return 1;
	}
	fwrite( resultbuffer, resultsize, 1, o );
	fclose(o);
	free( workbuffer );
	free( resultbuffer );
	
	fclose(f);
	printf("Done.\n");
	return 0;
}
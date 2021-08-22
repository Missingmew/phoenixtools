#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "ntrcom/nitrocompression.h"

int main( int argc, char** argv ) {
	
	if ( argc < 4 )
	{
		printf( "Not enough arguments given!\nUsage: %s [file] [offset in hex] [size in hexbytes]\nFiles will be extracted in current working directory.\n", argv[0] );
		return 1;
	}
	
	unsigned int givenOffset, givenSize, resultsize, compressedsize;
	givenOffset = strtoul(argv[2], NULL, 16);
	givenSize = strtoul(argv[3], NULL, 16);
	char outputname[512] = { 0 };
	unsigned char *workbuffer = NULL, *resultbuffer = NULL;
	FILE *f, *o;
	
	if( !(f = fopen( argv[1], "rb" ))) {
		printf("Couldnt open file %s\n", argv[1]);
		return 1;
	}
	
	fseek( f, givenOffset, SEEK_SET );
	
	printf("Current file offset: %08x - Current file size %08x\n", givenOffset, givenSize);
	workbuffer = malloc(givenSize);
	fread( workbuffer, givenSize, 1, f );
	compressedsize = givenSize;
	resultbuffer = unpackBuffer( workbuffer, &resultsize, &compressedsize );
	if(!resultbuffer) {
		printf("file is uncompressed!\n");
		snprintf( outputname, 512, "%08d-%08x-uncompressed.bin", givenOffset, givenOffset );
		resultsize = 0;
	}
	else snprintf( outputname, 512, "%08d-%08x-compressed.bin", givenOffset, givenOffset );
	if( !(o = fopen( outputname, "wb" ))) {
		printf("Couldnt open file %s\n", outputname);
		return 1;
	}
	if(!resultsize) fwrite( workbuffer, givenSize, 1, o );
	else fwrite( resultbuffer, resultsize, 1, o );
	fclose(o);
	
	free( workbuffer );
	if(resultsize) free( resultbuffer );
	
	printf("Done.\n");
	fclose(f);
	return 0;
	
}

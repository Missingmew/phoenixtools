#include <stdio.h>
#include <stdlib.h>
#include "nitrocompression.h"

int main( int argc, char **argv ) {
	FILE *f, *o;
	unsigned int filesize, resultsize, compressedsize;
	char fileName[512];
	unsigned char *workbuffer, *resultbuffer;
	if( argc < 2 ) {
		printf("Not enough args!\nUse: %s file\n", argv[0]);
		return 1;
	}
	
	if( !(f = fopen( argv[1], "rb" ))) {
		printf("Couldnt open file %s\n", argv[1]);
		return 1;
	}
	
	fseek( f, 0, SEEK_END );
	filesize = ftell(f);
	fseek( f, 0, SEEK_SET );
	
	sprintf( fileName, "%s.unpak", argv[1] );
	workbuffer = malloc( filesize );
	fread( workbuffer, filesize, 1, f );
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
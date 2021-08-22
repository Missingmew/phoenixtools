#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "ntrcom/nitrocompression.h"

typedef struct {
	uint32_t offset;
	uint32_t size;
}__attribute__((packed)) pacEntry;

int main( int argc, char** argv ) {
	
	if ( argc < 4 )
	{
		printf( "Not enough arguments given!\nUsage: %s [file] [offset in hex] [ignore empty files]\nFiles will be extracted in current working directory.\n", argv[0] );
		return 1;
	}
	
	uint32_t i, j, numFiles, listOffset, givenOffset, ignoreEmpty;
	unsigned int resultsize, compressedsize, checksum;
	givenOffset = strtoul(argv[2], NULL, 16);
	ignoreEmpty = strtoul(argv[3], NULL, 10);
	char outputname[32] = { 0 };
	unsigned char *workbuffer = NULL, *resultbuffer = NULL;
	pacEntry workentry;
	FILE *f, *o;
	
	if( !(f = fopen( argv[1], "rb" ))) {
		printf("Couldnt open file %s\n", argv[1]);
		return 1;
	}
	
	fseek( f, givenOffset, SEEK_SET );
	fread( &numFiles, 4, 1, f );
	
	for( i = 0; i < numFiles; i++ ) {
		checksum = 0;
		fread( &workentry, sizeof(pacEntry), 1, f );
		printf("Current file offset: %08x - Current file size %08x\n", givenOffset + workentry.offset, workentry.size);
		listOffset = ftell(f);
		fseek( f, givenOffset + workentry.offset, SEEK_SET );
		snprintf( outputname, 32, "%08d-%08x.bin", i, i );
		workbuffer = malloc(workentry.size);
		fread( workbuffer, workentry.size, 1, f );
		compressedsize = workentry.size;
		resultbuffer = unpackBuffer( workbuffer, &resultsize, &compressedsize );
		if(!resultbuffer) {
			printf("file %08x is uncompressed!\n", i);
			/* fseek( f, givenOffset + workentry.offset, SEEK_SET );
			fread( workbuffer, workentry.size, 1, f ); */
			resultsize = 0;
		}
		for( j = 0; j < resultsize; j++ ) checksum += resultbuffer[j];
		if( checksum || !ignoreEmpty ) {
			if( !(o = fopen( outputname, "wb" ))) {
				printf("Couldnt open file %s\n", outputname);
				return 1;
			}
			if(!resultsize) fwrite( workbuffer, workentry.size, 1, o );
			else fwrite( resultbuffer, resultsize, 1, o );
			fclose(o);
		}
		else printf("file is completely made up of nullbytes, skipping...\n");
		free( workbuffer );
		if(resultsize) free( resultbuffer );
		fseek( f, listOffset, SEEK_SET );
	}
	printf("end of archive at %08x\n", givenOffset + workentry.offset + workentry.size );
	printf("Done.\n");
	fclose(f);
	return 0;
	
}

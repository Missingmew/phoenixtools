#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "ntrcom/nitrocompression.h"

typedef struct {
	uint32_t offset;
	uint32_t null;
	uint32_t unpaksize;
	uint32_t paksize;
	uint32_t unknown;
}__attribute__((packed)) pakEntry;

void createFileName( unsigned char *buffer, char *name, unsigned int i ) {
	// if( buffer[0] == 0x42 && buffer[1] == 0x43 && buffer[2] == 0x48 && buffer[3] == 0 ) snprintf( name, 32, "%08d-%08x.bch", i, i ); // BCH
	if( !strncmp( (char *)buffer, "BCH", 3 )) snprintf( name, 32, "%08d-%08x.bch", i, i ); // BCH
	else if( !strncmp( (char *)buffer, "FFNT", 4 )) snprintf( name, 32, "%08d-%08x.bcfnt", i, i ); // FFNT
	else if( !strncmp( (char *)buffer, ".ans", 4 )) snprintf( name, 32, "%08d-%08x.ans", i, i ); // .ans
	else if( !strncmp( (char *)buffer, "locm", 4 )) snprintf( name, 32, "%08d-%08x.locm", i, i ); // .locm
	else if ((i == 58) || (i == 6786) || (i == 9622)) snprintf( name, 32, "%08d-%08x.pxs", i, i ); // phoenix scripts as found in the NDS games
	else snprintf( name, 32, "%08d-%08x.bin", i, i );
	return;
}

int main( int argc, char** argv ) {
	
	if ( argc < 3 )
	{
		printf( "Not enough arguments given!\nUsage: %s pack.inc pack.dat\nFiles will be extracted in current working directory.\n", argv[0] );
		return 1;
	}
	
	unsigned int resultsize, compressedsize, i;
	char outputname[32] = { 0 };
	unsigned char *workbuffer = NULL, *resultbuffer = NULL;
	pakEntry workentry;
	FILE *inc, *pak, *o;
	
	if( !(inc = fopen( argv[1], "rb" ))) {
		printf("Couldnt open file %s\n", argv[1]);
		return 1;
	}
	if( !(pak = fopen( argv[2], "rb" ))) {
		printf("Couldnt open file %s\n", argv[2]);
		return 1;
	}
	
	i = 0;
	
	while( fread( &workentry, sizeof(pakEntry), 1, inc )) {
		printf("Current file number %08d - Current file offset: %08x\nCurrent compressed size %08x - Current uncompressed size %08x\n", i, workentry.offset, workentry.paksize, workentry.unpaksize);
		fseek( pak, workentry.offset, SEEK_SET );
		workbuffer = malloc( workentry.paksize );
		fread( workbuffer, workentry.paksize, 1, pak );
		resultbuffer = unpackBuffer( workbuffer, &resultsize, &compressedsize );
		if( !resultbuffer ) {
			printf("File is uncompressed!\n");
			createFileName( workbuffer, outputname, i );
			resultsize = 0;
		}
		else {
			if( !(resultsize == workentry.unpaksize)) printf( "Uncompressed buffer size does not match recorded buffer size (%08x - %08x)!\n", resultsize, workentry.unpaksize);
			createFileName( resultbuffer, outputname, i );
		}
		if( !(o = fopen( outputname, "wb" ))) {
				printf("Couldnt open file %s\n", outputname);
				return 1;
		}
		if( resultsize ) fwrite( resultbuffer, resultsize, 1, o );
		else fwrite( workbuffer, workentry.paksize, 1, o );
		fclose(o);
		i++;
	}
	printf("Done.\n");
	fclose(inc);
	fclose(pak);
	return 0;
	
}

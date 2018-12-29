#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "ntrcom/nitrocompression.h"
#include "phoenixgfx.h"
#include "lodepng/lodepng.h"

#define BREAK(x) { printf(x); return 1; }

typedef struct {
	uint32_t offset;
	uint32_t size;
}__attribute__((packed)) pacEntry;

int main( int argc, char** argv ) {
	
	if ( argc < 5 )
	{
		printf( "Not enough arguments given!\nUsage: %s [file] [offset in hex] [tilesx] [tilesy] [type] [offset of source palette]\nType is:\n0 - new video type\n1 - old video type (requires offset of source palette)\nFiles will be extracted in current working directory.\n", argv[0] );
		return 1;
	}
	
	uint32_t i, numFiles, listOffset, givenOffset;
	unsigned int resultsize, compressedsize, tilesx, tilesy, type, sourceoffset, bpp;
	givenOffset = strtoul(argv[2], NULL, 16);
	tilesx = strtoul(argv[3], NULL, 10);
	tilesy = strtoul(argv[4], NULL, 10);
	type = strtoul(argv[5], NULL, 10);
	if(type) sourceoffset = strtoul(argv[6], NULL, 16);
	char outputname[32] = { 0 };
	unsigned char *workbuffer = NULL, *resultbuffer = NULL, *rgbaPixelData = NULL, *paletteData = NULL;
	pacEntry workentry, helpentry;
	FILE *f;
	
	if( !(f = fopen( argv[1], "rb" ))) {
		printf("Couldnt open file %s\n", argv[1]);
		return 1;
	}
	
	fseek( f, givenOffset, SEEK_SET );
	fread( &numFiles, 4, 1, f );
	switch(type) {
		case 0: {
			for( i = 0; i < numFiles; i++ ) {
				fread( &workentry, sizeof(pacEntry), 1, f );
				printf("Current file offset: %08x - Current file size %08x\n", givenOffset + workentry.offset, workentry.size);
				listOffset = ftell(f);
				fseek( f, givenOffset + workentry.offset, SEEK_SET );
				snprintf( outputname, 32, "%08d-%08x.png", i, i );
				workbuffer = malloc(workentry.size);
				fread( workbuffer, workentry.size, 1, f );
				resultbuffer = unpackBuffer( workbuffer, &resultsize, &compressedsize );
				// generates a RGBA image
				rgbaPixelData = tiledImageToRGBA( resultbuffer, tilesx, tilesy, image4bpp, 1 );
				
				lodepng_encode32_file( outputname, rgbaPixelData, tilesx*8, tilesy*8 );
				
				free( workbuffer );
				free( resultbuffer );
				free( rgbaPixelData );
				fseek( f, listOffset, SEEK_SET );
			}
			break;
		}
		case 1: {
			fseek( f, sourceoffset+4, SEEK_SET );
			fread( &helpentry, sizeof(pacEntry), 1, f );
			paletteData = malloc(helpentry.size);
			bpp = (helpentry.size == 32)?image4bpp:image8bpp;
			fseek( f, sourceoffset + helpentry.offset, SEEK_SET );
			fread( paletteData, helpentry.size, 1, f );
			fseek( f, givenOffset+4, SEEK_SET );
			for( i = 0; i < numFiles; i++ ) {
				fread( &workentry, sizeof(pacEntry), 1, f );
				printf("Current file offset: %08x - Current file size %08x\n", givenOffset + workentry.offset, workentry.size);
				listOffset = ftell(f);
				fseek( f, givenOffset + workentry.offset, SEEK_SET );
				snprintf( outputname, 32, "%08d-%08x.png", i, i );
				workbuffer = malloc(workentry.size);
				fread( workbuffer, workentry.size, 1, f );
				resultbuffer = unpackBuffer( workbuffer, &resultsize, &compressedsize );
				if(bpp == image8bpp) {
					if((tilesx*8*tilesy*8) != resultsize) BREAK("size dont match with uncompressed size\n");
				}
				else if(bpp == image4bpp) {
					if(((tilesx*8*tilesy*8)/2) != resultsize) BREAK("size dont match with uncompressed size\n");
				}
				// generates a RGBA image
				//~ rgbaPixelData = tiledImageToRGBA( resultbuffer, tilesx, tilesy, bpp, 1 );
				rgbaPixelData = linearImageWithPaletteToRGBA( resultbuffer, paletteData, tilesx*8, tilesy*8, bpp, 1 );
				
				lodepng_encode32_file( outputname, rgbaPixelData, tilesx*8, tilesy*8 );
				
				free( workbuffer );
				free( resultbuffer );
				free( rgbaPixelData );
				fseek( f, listOffset, SEEK_SET );
			}
			free(paletteData);
			break;
		}
		default: {
			printf("argh\n");
			return 1;
		}
	}
	printf("end of video at %08x\n", givenOffset + workentry.offset + workentry.size );
	printf("bpp was %d %u\n", (bpp == image4bpp)?4:8, helpentry.size);
	printf("Done.\n");
	fclose(f);
	return 0;
	
}

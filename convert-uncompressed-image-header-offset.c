#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "ntrcom/nitrocompression.h"
#include "phoenixgfx.h"
#include "lodepng/lodepng.h"

#define BREAK(x) { printf(x); return 1; }

typedef struct {
	unsigned char magic[4];
	uint32_t headerSize;
	uint32_t imageSize;
	uint32_t paletteOffset;
	uint32_t paletteSize;
}__attribute__((packed)) controlHeader;

/* these files seem to be based around the common nitro BTX0/NSBTX format... */

int main( int argc, char** argv ) {
	
	if ( argc < 6 )
	{
		printf( "Not enough arguments given!\nUsage: %s [file] [offset in hex] [pixelsX] [pixelsY] [transparency] [alpha override]\nnote that the pixelsX and pixelsY arguments are for fallback purposes only, the sizes are usually in the file\nalpha override is optional, pass the number of non alpha pixels in hex (works only with 8bpp!)\n", argv[0] );
		return 1;
	}
	
	uint32_t givenOffset;
	unsigned int i, transparency, pixelsX, pixelsY, bpp, alphaover;
	givenOffset = strtoul(argv[2], NULL, 16);
	pixelsX = strtoul(argv[3], NULL, 10);
	pixelsY = strtoul(argv[4], NULL, 10);
	transparency = strtoul(argv[5], NULL, 10);
	if(argc == 7) alphaover = strtoul(argv[6], NULL, 16);
	
	char outputname[32] = { 0 };
	unsigned char *workbuffer = NULL, *rgbaPixelData = NULL, *paletteData = NULL;
	controlHeader header;
	FILE *f;
	
	if( !(f = fopen( argv[1], "rb" ))) {
		printf("Couldnt open file %s\n", argv[1]);
		return 1;
	}
	
	fseek( f, givenOffset, SEEK_SET );
	fread( &header, 0x14, 1, f );
	if( header.magic[1] > 5 ) printf("evil width %x in file\n", header.magic[1]);
	else { 
		pixelsX = 8 << header.magic[1];
		printf("read tex width is %04d\n", pixelsX);
	}
	if( header.magic[2] > 5 ) printf("evil height %x in file\n", header.magic[2]);
	else {
		pixelsY = 8 << header.magic[2];
		printf("read tex height is %04d\n", pixelsY);
	}
	/* byte 0 is the texture type, refer to GBATEK->DS 3D Texture Formats for details */
	switch( header.magic[0] ) {
		/* 4bpp */
		case 3: {
			printf("image is 4bpp\n");
			bpp = image4bpp;
			if((header.imageSize) != ((pixelsX*pixelsY)/2)) BREAK("size doesnt match!\n");
			break;
		}
		/* 8bpp */
		case 4: {
			printf("image is 8bpp\n");
			bpp = image8bpp;
			if((header.imageSize) != ((pixelsX*pixelsY))) BREAK("size doesnt match!\n");
			break;
		}
		/* a5i3 */
		case 6: {
			printf("image is a5i3\n");
			bpp = imagea5i3;
			if((header.imageSize) != ((pixelsX*pixelsY))) BREAK("size doesnt match!\n");
			break;
		}
		/* fallback */
		default: {
			printf("fallback. textype is %08x\n", header.magic[0]);
			if( header.paletteSize < 0x200 ) {
				printf("image is 4bpp\n");
				bpp = image4bpp;
				if((header.imageSize) != ((pixelsX*pixelsY)/2)) BREAK("size doesnt match!\n");
			}
			else {
				printf("image is 8bpp\n");
				bpp = image8bpp;
				if((header.imageSize) != ((pixelsX*pixelsY))) BREAK("size doesnt match!\n");
			}
			break;
		}
	}
	if( alphaover && !image8bpp ) BREAK("alphaover only works with 8bpp images!\n");
	if( bpp == imagea5i3 ) {
		if( header.paletteSize == 0x200 ) paletteData = malloc(header.paletteSize);
		else {
			paletteData = malloc(8*2);
			memset(paletteData, 0, 8*2);
		}
	}
	else if ( bpp == imagea3i5 ) {
		paletteData = malloc(32*2);
		memset(paletteData, 0, 32*2);
	}
	else paletteData = malloc(header.paletteSize);
	fseek( f, givenOffset+header.paletteOffset, SEEK_SET );
	fread( paletteData, header.paletteSize, 1, f );
	fseek( f, givenOffset+header.headerSize, SEEK_SET );
	workbuffer = malloc(header.imageSize);
	fread( workbuffer, header.imageSize, 1, f );
	if( alphaover ) for( i = alphaover; i < header.imageSize; i++ ) workbuffer[i] = 0;
	
	rgbaPixelData = linearImageWithPaletteToRGBA(workbuffer, paletteData, pixelsX, pixelsY, bpp, transparency);
	
	snprintf( outputname, 32, "%08d-%08x.png", givenOffset, givenOffset );
	lodepng_encode32_file( outputname, rgbaPixelData, pixelsX, pixelsY );
	free( workbuffer );
	free( rgbaPixelData );
	free( paletteData );
	printf("total size of image %08x\n", header.paletteOffset + header.paletteSize);
	
	printf("Done.\n");
	fclose(f);
	return 0;
	
}

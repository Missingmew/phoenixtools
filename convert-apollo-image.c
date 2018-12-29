#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "ntrcom/nitrocompression.h"
#include "phoenixgfx.h"
#include "lodepng/lodepng.h"

#define BREAK(x) { printf(x); return 1; }

int main( int argc, char** argv ) {
	
	if ( argc < 4 )
	{
		printf( "Not enough arguments given!\nUsage: %s [infile] [outfile] [transparency]\n", argv[0] );
		return 1;
	}
	
	FILE *f;
	uint32_t head;
	int pixelsx, pixelsy, tilesx, tilesy, palettesize, imagesize, bpp, alpha;
	unsigned char *workbuf, *rgbaPixelData;
	
	alpha = strtoul(argv[3], NULL, 10);
	
	if( !(f = fopen( argv[1], "rb" ))) {
		printf("Couldnt open file %s\n", argv[1]);
		return 1;
	}
	
	fread( &head, 4, 1, f );
	bpp = (head & (1<<31))?image4bpp:image8bpp;
	pixelsx = head & 0xffff;
	pixelsy = (head & 0x7fff0000) >> 16;
	printf("bpp is %d, width is %04d, height is %04d\n", bpp, pixelsx, pixelsy);
	tilesx = pixelsx / 8;
	tilesy = pixelsy / 8;
	imagesize = pixelsx*pixelsy;
	palettesize = 512;
	if(bpp == image4bpp) {
		imagesize /= 2;
		palettesize = 32;
	}
	workbuf = malloc(palettesize+imagesize);
	fread(workbuf, palettesize+imagesize, 1, f);
	rgbaPixelData = tiledImageToRGBA(workbuf, tilesx, tilesy, bpp, alpha);
	lodepng_encode32_file( argv[2], rgbaPixelData, pixelsx, pixelsy );
	free(workbuf);
	free(rgbaPixelData);
	fclose(f);
	printf("Done.\n");
	return 0;
}	
	
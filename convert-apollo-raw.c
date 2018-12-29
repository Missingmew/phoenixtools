#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "ntrcom/nitrocompression.h"
#include "phoenixgfx.h"
#include "lodepng/lodepng.h"

#define BREAK(x) { printf(x); return 1; }

int main( int argc, char** argv ) {
	
	if ( argc < 7 )
	{
		printf( "Not enough arguments given!\nUsage: %s [infile] [outfile] [tilesx] [tilesy] [bpp as 4/8] [transparency] [[palette file] [palette offset in file]]\n", argv[0] );
		return 1;
	}
	
	FILE *f, *p;
	int tilesx, tilesy, palettesize, imagesize, bpp, alpha, paletteoffset, haspalette = 0, linear = 0;
	unsigned char *imagebuf, *palettebuf, *rgbaPixelData;
	tilesx = strtoul(argv[3], NULL, 10);
	tilesy = strtoul(argv[4], NULL, 10);
	bpp = strtoul(argv[5], NULL, 10);
	alpha = strtoul(argv[6], NULL, 10);
	
	if( !(f = fopen( argv[1], "rb" ))) {
		printf("Couldnt open file %s\n", argv[1]);
		return 1;
	}
	if( argc == 9 ) {
		if( !(p = fopen(argv[7], "rb" ))) {
			printf("Couldnt open file %s\n", argv[7]);
			return 1;
		}
		paletteoffset = strtoul(argv[8], NULL, 16);
		haspalette=1;
		fseek( p, 0, SEEK_END );
		palettesize = ftell(p);
		fseek( p, paletteoffset, SEEK_SET );
	}
	if( ((bpp - 10) > 0) && haspalette ) {
		linear = 1;
		bpp -= 10;
	}
	else if( ((bpp - 10) > 0) && !haspalette ) BREAK("linear mode only works with seperate palette\n");
	fseek( f, 0, SEEK_END );
	imagesize = ftell(f);
	fseek( f, 0, SEEK_SET );
	if( bpp == 4 ) {
		bpp = image4bpp;
		if(haspalette) {
			printf("imagesize %08x, sizeoftiles %08x\n", imagesize, (tilesx*8*tilesy*8)/2);
			if( (tilesx*8*tilesy*8)/2 != imagesize ) BREAK("image size doesnt match bpp\n");
			if( (palettesize-paletteoffset) < 32 ) BREAK("palette is too small!\n");
		}
		else {
			printf("imagesize %08x, sizeoftiles %08x, image wo palette %08x\n", imagesize, (tilesx*8*tilesy*8)/2, imagesize-32);
			if( (tilesx*8*tilesy*8)/2 != (imagesize-32) ) BREAK("image size doesnt match bpp\n");
		}
		palettesize=32;
	}
	else if ( bpp == 8 ) {
		bpp = image8bpp;
		if(haspalette) {
			printf("imagesize %08x, sizeoftiles %08x\n", imagesize, (tilesx*8*tilesy*8));
			if( (tilesx*8*tilesy*8) != imagesize ) BREAK("image size doesnt match bpp\n");
			if( (palettesize-paletteoffset) < 512 ) BREAK("palette is too small!\n");
		}
		else {
			printf("imagesize %08x, sizeoftiles %08x, image wo palette %08x\n", imagesize, (tilesx*8*tilesy*8), imagesize-512);
			if( (tilesx*8*tilesy*8) != (imagesize-512) ) BREAK("image size doesnt match bpp\n");
		}
		palettesize=512;
	}
	else BREAK("unsupported bpp, use 4 or 8\n");
	
	imagebuf = malloc(imagesize);
	fread(imagebuf, imagesize, 1, f);
	if(haspalette) {
		if( alpha == 2 ) BREAK("you dun goofed\n");
		palettebuf = malloc(palettesize);
		fread(palettebuf, palettesize, 1, p);
		if (linear) rgbaPixelData = linearImageWithPaletteToRGBA(imagebuf, palettebuf, tilesx*8, tilesy*8, bpp, alpha);
		else rgbaPixelData = tiledImageWithPaletteToRGBA(imagebuf, palettebuf, tilesx, tilesy, bpp, alpha);
		free(palettebuf);
	}
	else if( alpha == 2 ) {
		alpha--;
		palettebuf = malloc(palettesize);
		fread(palettebuf, palettesize, 1, f);
		rgbaPixelData = tiledImageWithPaletteToRGBA(imagebuf, palettebuf, tilesx, tilesy, bpp, alpha);
		free(palettebuf);
	}
	else rgbaPixelData = tiledImageToRGBA(imagebuf, tilesx, tilesy, bpp, alpha);
	lodepng_encode32_file( argv[2], rgbaPixelData, tilesx*8, tilesy*8 );
	free(imagebuf);
	free(rgbaPixelData);
	fclose(f);
	printf("Done.\n");
	return 0;
}	
	
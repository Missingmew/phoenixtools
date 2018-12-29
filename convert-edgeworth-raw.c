#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "ntrcom/nitrocompression.h"
#include "phoenixgfx.h"
#include "lodepng/lodepng.h"

#define BREAK(x) { printf(x); return 1; }

int main( int argc, char** argv ) {
	
	if ( argc < 9 )
	{
		printf( "Not enough arguments given!\nUsage: %s [infile] [outfile] [tilesx] [tilesy] [bpp as 4/8] [transparency] [palette file] [palette number] [tilemap-optional] [maptype-optional]\npalette number is ignored if using a tilemap\n", argv[0] );
		return 1;
	}
	
	FILE *f = NULL, *p = NULL, *m = NULL;
	unsigned int tilesx, tilesy, palettesize, mapsize, imagesize, alpha, paletteoffset = 0, linear = 0, eaf = 0, maptype = 0, hasmap = 0;
	int bpp;
	unsigned char *imagebuf = NULL, *palettebuf = NULL, *rgbaPixelData = NULL;
	uint16_t *tilemap16 = NULL;
	uint8_t *tilemap8 = NULL;
	tilesx = strtoul(argv[3], NULL, 10);
	tilesy = strtoul(argv[4], NULL, 10);
	bpp = strtoul(argv[5], NULL, 10);
	alpha = strtoul(argv[6], NULL, 10);
	paletteoffset = strtoul(argv[8], NULL, 10);
	
	if( !(f = fopen( argv[1], "rb" ))) {
		printf("Couldnt open file %s as source\n", argv[1]);
		return 1;
	}
	if( !(p = fopen(argv[7], "rb" ))) {
		printf("Couldnt open file %s as palette\n", argv[7]);
		return 1;
	}
	
	if( argc > 9 ) {
		if( argc < 11 ) {
			printf( "Not enough arguments given!\nUsage: %s [infile] [outfile] [tilesx] [tilesy] [bpp as 4/8] [transparency] [palette file] [palette number] [tilemap-optional] [maptype-optional]\n", argv[0] );
			return 1;
		}
		if( !(m = fopen(argv[9], "rb" ))) {
			printf("Couldnt open file %s as tilemap\n", argv[9]);
			return 1;
		}
		maptype = strtoul(argv[10], NULL, 10);
		fseek(m, 0, SEEK_END);
		palettesize = ftell(m);
		if( maptype ) {
			printf("total tiles %08x - num tiles in map %08x\n", tilesx*tilesy, palettesize/2);
			if( tilesx*tilesy*2 != palettesize ) BREAK("num tiles specified doesnt match tiles in map!\n");
			fseek(m, 0, SEEK_SET);
			tilemap16 = malloc(palettesize*sizeof(uint16_t));
			fread(tilemap16, palettesize*sizeof(uint16_t), 1, m);
			mapsize = palettesize/2;
		}
		else {
			printf("total tiles %08x - num tiles in map %08x\n", tilesx*tilesy, palettesize);
			if( tilesx*tilesy != palettesize ) BREAK("num tiles specified doesnt match tiles in map!\n");
			fseek(m, 0, SEEK_SET);
			tilemap8 = malloc(palettesize);
			fread(tilemap8, palettesize, 1, m);
			mapsize = palettesize;
		}
		fclose(m);
		hasmap = 1;
	}
	
	fseek( p, 0, SEEK_END );
	palettesize = ftell(p);
	fseek( p, 0, SEEK_SET );
	
	if( bpp == 48 ) {
		if( palettesize < 32 ) BREAK("palette is too small\n");
		eaf = 1;
		palettesize = 32;
		bpp = 8;
	}
	
	if((bpp - 10) > 0) {
		linear = 1;
		bpp -= 10;
	}

	fseek( f, 0, SEEK_END );
	imagesize = ftell(f);
	fseek( f, 0, SEEK_SET );
	if( bpp == 4 ) {
		bpp = image4bpp;
		printf("imagesize %08x, sizeoftiles %08x\n", imagesize, (tilesx*8*tilesy*8)/2);
		if(!hasmap) if( ((tilesx*8*tilesy*8)/2) != imagesize ) BREAK("image size doesnt match bpp\n");
		if( (palettesize-paletteoffset) < 32 ) BREAK("palette is too small!\n");
		if((32*paletteoffset) > palettesize) BREAK("selected a higher then possible palette!\n");
		printf("num tiles in image %08x\n", imagesize / 0x20);
		palettesize=32;
	}
	else if ( bpp == 8 ) {
		if(paletteoffset) BREAK("only 4bpp support multipalette!\n");
		bpp = image8bpp;
		printf("imagesize %08x, sizeoftiles %08x\n", imagesize, (tilesx*8*tilesy*8));
		if(!hasmap) if( (tilesx*8*tilesy*8) != imagesize ) BREAK("image size doesnt match bpp\n");
		if(!eaf) {
			if( (palettesize-paletteoffset) < 512 ) BREAK("palette is too small!\n");
			palettesize=512;
		}
		printf("num tiles in image %08x\n", imagesize / 0x40);
	}
	else BREAK("unsupported bpp, use 4 or 8\n");
	
	
	imagebuf = malloc(imagesize);
	fread(imagebuf, imagesize, 1, f);
	palettebuf = malloc(palettesize);
	fseek(p, 32*paletteoffset, SEEK_SET);
	if(eaf) {
		memset(palettebuf, 0, palettesize);
		palettesize = 32;
	}
	fread(palettebuf, palettesize, 1, p);
	fflush(stdout);
	if(linear) rgbaPixelData = linearImageWithPaletteToRGBA(imagebuf, palettebuf, tilesx*8, tilesy*8, bpp, alpha);
	else if(hasmap && tilemap16) rgbaPixelData = tiledImageWithPaletteAndTilesetToRGBA(imagebuf, palettebuf, tilemap16, tilesx, tilesy, bpp, alpha, maptype, mapsize);
	else if(hasmap && tilemap8) rgbaPixelData = tiledImageWithPaletteAndTilesetToRGBA(imagebuf, palettebuf, tilemap8, tilesx, tilesy, bpp, alpha, maptype, mapsize);
	else rgbaPixelData = tiledImageWithPaletteToRGBA(imagebuf, palettebuf, tilesx, tilesy, bpp, alpha);
	free(palettebuf);
	
	lodepng_encode32_file( argv[2], rgbaPixelData, tilesx*8, tilesy*8 );
	free(imagebuf);
	free(rgbaPixelData);
	if(hasmap && tilemap16) free(tilemap16);
	else if(hasmap && tilemap8) free(tilemap8);
	fclose(f);
	printf("Done.\n");
	return 0;
}	
	
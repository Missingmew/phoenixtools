#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define BREAK(x) { printf(x); return 1; }

typedef struct {
	uint16_t offset;
	uint16_t size;
	uint32_t unknown;
}__attribute__((packed)) headerentry;

typedef struct {
	unsigned int y:8;
	unsigned int rotscale:1;
	unsigned int doubledis:1;
	unsigned int mode:2;
	unsigned int mosaic:1;
	unsigned int colors:1;
	unsigned int shape:2;
}__attribute__((packed)) oam0;

/* if oam0 rotscale is set, use oam1wrs, if not set, use oam1nrs */

typedef struct {
	unsigned int x:9;
	unsigned int rotscaleparam:5;
	unsigned int size:2;
}__attribute__((packed)) oam1wrs;

typedef struct {
	unsigned int x:9;
	unsigned int unused:3;
	unsigned int hflip:1;
	unsigned int vflip:1;
	unsigned int size:2;
}__attribute__((packed)) oam1nrs;

typedef struct {
	unsigned int tilenum:10;
	unsigned int priority:2;
	unsigned int palnum:4;
}__attribute__((packed)) oam2;

/* add things about rotation, scaling params, PA, PB, PC, PD */

int main( int argc, char** argv ) {
	
	if ( argc < 2 )
	{
		printf( "Not enough arguments given!\nUsage: %s [infile]\n", argv[0] );
		return 1;
	}
	
	FILE *f;
	headerentry head;
	headerentry *headlist = NULL;
	
	uint32_t head;
	int pixelsx, pixelsy, tilesx, tilesy, palettesize, imagesize, bpp, alpha;
	unsigned char *workbuf, *rgbaPixelData;
	
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
	
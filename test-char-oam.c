#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define BREAK(x) { printf(x); return 1; }

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

typedef union {
	oam1wrs o1w;
	oam1nrs o1n;
} oam1;

typedef struct {
	unsigned int tilenum:10;
	unsigned int priority:2;
	unsigned int palnum:4;
}__attribute__((packed)) oam2;

/* add things about rotation, scaling params, PA, PB, PC, PD */

int main( int argc, char** argv ) {
	
	if ( argc < 3 )
	{
		printf( "Not enough arguments given!\nUsage: %s [infile] [offset]\n", argv[0] );
		return 1;
	}
	
	FILE *f;
	
	unsigned int off = strtoul(argv[2], NULL, 16);
	oam0 o0;
	oam1 o1;
	oam2 o2;
	
	if( !(f = fopen( argv[1], "rb" ))) {
		printf("Couldnt open file %s\n", argv[1]);
		return 1;
	}
	
	fseek(f, off, SEEK_SET);
	
	fread(&o0, sizeof(oam0), 1, f);
	fread(&o1, sizeof(oam1), 1, f);
	fread(&o2, sizeof(oam2), 1, f);
	
	printf("OAM0: y %d, rot %d, doubledis %d, mode %d, mosaic %d, color %d, shape %d\n", o0.y, o0.rotscale, o0.doubledis, o0.mode, o0.mosaic, o0.colors, o0.shape);
	printf("OAM1 ");
	if(o0.rotscale) {
		printf("with rotscale: x %d, rotscaleparam %d, size %d\n", o1.o1w.x, o1.o1w.rotscaleparam, o1.o1w.size);
	}
	else {
		printf("no rotscale: x %d, nouse %d, hflip %d, vflip %d, size %d\n", o1.o1n.x, o1.o1n.unused, o1.o1n.hflip, o1.o1n.vflip, o1.o1n.size);
	}
	printf("OAM2: tilenum %d(%02x), prio %d, palnum %d\n", o2.tilenum, o2.tilenum, o2.priority, o2.palnum);
	
	fclose(f);
	printf("Done.\n");
	return 0;
}	
	
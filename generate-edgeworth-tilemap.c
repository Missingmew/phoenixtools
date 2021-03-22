#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define BREAK(x) { printf(x); return 1; }

int main( int argc, char** argv ) {
	
	if ( argc < 3 )
	{
		printf( "Not enough arguments given!\nUsage: %s textmap binmap\n", argv[0] );
		return 1;
	}
	
	unsigned int i, numtiles, tileindex, hflip, vflip, palnum;
	uint16_t *tilemap = NULL;
	FILE *o, *t;
	
	if( !(t = fopen( argv[1], "r" ))) {
		printf("Couldnt open file %s as textmap\n", argv[1]);
		return 1;
	}
	if( !(o = fopen(argv[2], "wb" ))) {
		printf("Couldnt open file %s as binmap\n", argv[2]);
		return 1;
	}
	
	fscanf(t, "%u\r\n", &numtiles);
	printf("numtiles %d\n", numtiles);
	tilemap = malloc(numtiles*sizeof(uint16_t));
	for(i = 0; i < numtiles; i++) {
		fscanf(t, "%u %u %u %u\r\n", &tileindex, &hflip, &vflip, &palnum);
		printf("tileindex %u - flip hv %d %d - palnum %d\n", tileindex, hflip, vflip, palnum);
		if( tileindex > 0x3FF ) BREAK("tileindex exceeds limit of 0x3FF\n");
		if( palnum > 15 ) BREAK("palnum exceeds limit of 15\n");
		tilemap[i] = (tileindex & 0x3FF) | ((hflip?1:0) << 10) | ((vflip?1:0) << 11) | ((palnum & 15) << 12);
	}
	
	fwrite(tilemap, numtiles*sizeof(uint16_t), 1, o);
	fclose(t);
	fclose(o);
	free(tilemap);
	printf("Done.\n");
	return 0;
}
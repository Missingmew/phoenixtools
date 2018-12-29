#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

typedef struct {
	unsigned char magic[4];
	uint32_t headerSize;
	uint32_t imageSize;
	uint32_t paletteOffset;
	uint32_t paletteSize;
}__attribute__((packed)) controlHeader;

/* these files seem to be based around the common nitro BTX0/NSBTX format... */

int main( int argc, char** argv ) {
	
	if ( argc < 4 )
	{
		printf( "Not enough arguments given!\nUsage: %s [file] [start offset] [totalsize]\n", argv[0] );
		return 1;
	}
	
	unsigned int start, end, totalsize, curoff, i = 0;
	unsigned char *sourcedata = NULL;
	start = strtoul(argv[2], NULL, 16);
	totalsize = strtoul(argv[3], NULL, 16);
	end = start+totalsize;
	
	//~ printf("start %08x size %08x end %08x\n", start, totalsize, end);
	
	controlHeader *header = NULL;
	FILE *f;
	
	if( !(f = fopen( argv[1], "rb" ))) {
		printf("Couldnt open file %s\n", argv[1]);
		return 1;
	}
	
	fseek( f, start, SEEK_SET );
	sourcedata = malloc(totalsize);
	fread(sourcedata, totalsize, 1, f);
	fclose(f);
	
	curoff = 0;
	
	while((start+curoff) < end) {
		header = (controlHeader *)(sourcedata+curoff);
		printf("0x%08x\t5\t\"uncomheader-%08x-%03d.png\"\n", curoff+start, start, i);
		curoff += (header->paletteOffset + header->paletteSize);
		i++;
	}
	
	free(sourcedata);
	//~ printf("Done.\n");
	return 0;
	
}

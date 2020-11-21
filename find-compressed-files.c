#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "ntrcom/nitrocompression.h"

int main( int argc, char** argv ) {
	
	unsigned int size, extract, allowuneven, offset, start = 0, end = 0, resultsize = 0, compressedsize = 0;
	
	if ( argc < 4 )
	{
		printf( "Not enough arguments given!\nUsage: %s [file] [map/extract] [allow uneven] [offset in hex] [end in hex]\nFiles will be extracted in current working directory.\n", argv[0] );
		return 1;
	}
	extract = strtoul(argv[2], NULL, 10);
	allowuneven = strtoul(argv[3], NULL, 10);
	if(argc > 4) start = strtoul(argv[4], NULL, 16);
	if(argc > 5) end = strtoul(argv[5], NULL, 16);
	
	char outputname[512] = { 0 };
	unsigned char *data = NULL, *resultbuffer = NULL;
	FILE *f, *o;
	
	if( !(f = fopen( argv[1], "rb" ))) {
		printf("Couldnt open file %s\n", argv[1]);
		return 1;
	}
	
	fseek(f, 0, SEEK_END);
	size = ftell(f);
	fseek(f, 0, SEEK_SET);
	data = malloc(size);
	fread(data, size, 1, f);
	fclose(f);
	f = NULL;
	
	offset = start;
	if(offset % 4) offset -= offset % 4;
	
	if(!end) end = size;
	if(end % 4) end -= end % 4;
	
	printf("extract %x offset %x end %x\n", extract, offset, end);
	
	
	while(offset < end) {
		//~ if((data[offset] >> 4) == 1) {
		if((data[offset]) == 0x10) { // only check for lz10
			resultsize = (data[offset+3] << 16) | (data[offset+2] << 8) | data[offset+1];
			if((!(resultsize % 2) || allowuneven) && resultsize < 0x10000) {
				compressedsize = resultsize * 2;
				resultbuffer = unpackBuffer(data+offset, &resultsize, &compressedsize);
				if(resultbuffer) {
					printf("%08X: 0x%08X hit compressed 0x%04X uncompressed 0x%04X\n", start, offset, compressedsize, resultsize);
					if(extract) {
						snprintf(outputname, 512, "%08X-compressed.bin.lz", offset);
						if( !(o = fopen( outputname, "wb" ))) {
							printf("Couldnt open file %s\n", outputname);
							return 1;
						}
						fwrite(data+offset, compressedsize, 1, o);
						fclose(o);
					}
					offset += compressedsize;
					free(resultbuffer);
				}
				else offset += 4;
			}
			else offset += 4;
		}
		else offset += 4;
		fflush(stderr);
		while(offset%4) offset++;
	}
	free(data);
	printf("\n");
	//~ printf("Done.\n");
	return 0;
	
}

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "ntrcom/nitrocompression.h"

int main( int argc, char** argv ) {
	
	unsigned int size, extract, offset, start = 0, end = 0, resultsize, compressedsize;
	
	if ( argc < 3 )
	{
		printf( "Not enough arguments given!\nUsage: %s [file] [map/extract] [offset in hex] [end in hex]\nFiles will be extracted in current working directory.\n", argv[0] );
		return 1;
	}
	extract = strtol(argv[2], NULL, 16);
	if(argc > 3) start = strtol(argv[3], NULL, 16);
	if(argc > 4) end = strtol(argv[4], NULL, 16);
	
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
	
	if(!end) end = size;
	
	printf("extract %x offset %x end %x\n", extract, offset, end);
	
	
	while(offset < end) {
		//~ fprintf(stderr, "offset %x\n", offset);
		//~ fflush(stderr);
		if((data[offset] >> 4) == 1) {
			compressedsize = (data[offset+3] << 16) | (data[offset+2] << 8) | data[offset+1];
			if(compressedsize < 0x10000) {
				compressedsize *= 2;
				resultbuffer = unpackBuffer(data+offset, &compressedsize, &resultsize);
				if(resultbuffer) {
					//~ printf("0x%08X hit compressed 0x%04X uncompressed 0x%04X\n", offset, compressedsize, resultsize);
					printf("%08X: 0x%08X hit compressed 0x%04X uncompressed 0x%04X\n", start, offset, compressedsize, resultsize);
					if(extract) {
						snprintf(outputname, 512, "%08X-compressed.bin", offset);
						if( !(o = fopen( outputname, "wb" ))) {
							printf("Couldnt open file %s\n", outputname);
							return 1;
						}
						fwrite(data+offset, compressedsize, 1, o);
						fclose(o);
						
						//~ snprintf(outputname, 512, "%08X-decompressed.bin", offset);
						//~ if( !(o = fopen( outputname, "wb" ))) {
							//~ printf("Couldnt open file %s\n", outputname);
							//~ return 1;
						//~ }
						//~ fwrite(resultbuffer, resultsize, 1, o);
						//~ fclose(o);
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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

unsigned char *LZDecompress(unsigned char *src, int *compressedSize, int *uncompressedSize) {
	unsigned char *dest = NULL;
	int srcSize = *compressedSize;
	if (srcSize < 4)
		goto fail;

	int destSize = (src[3] << 16) | (src[2] << 8) | src[1];

	dest = malloc(destSize);

	if (dest == NULL)
		goto fail;

	int srcPos = 4;
	int destPos = 0;

	for (;;) {
		if (srcPos >= srcSize)
			goto fail;

		unsigned char flags = src[srcPos++];

		for (int i = 0; i < 8; i++) {
			if (flags & 0x80) {
				if (srcPos + 1 >= srcSize)
					goto fail;

				int blockSize = (src[srcPos] >> 4) + 3;
				int blockDistance = (((src[srcPos] & 0xF) << 8) | src[srcPos + 1]) + 1;

				srcPos += 2;

				int blockPos = destPos - blockDistance;

				// Some Ruby/Sapphire tilesets overflow.
				if (destPos + blockSize > destSize) {
					blockSize = destSize - destPos;
					//~ fprintf(stderr, "Destination buffer overflow.\n");
				}

				if (blockPos < 0)
					goto fail;

				for (int j = 0; j < blockSize; j++)
					dest[destPos++] = dest[blockPos + j];
			} else {
				if (srcPos >= srcSize || destPos >= destSize)
					goto fail;

				dest[destPos++] = src[srcPos++];
			}

			if (destPos == destSize) {
				while(srcPos%4) srcPos++;
				*compressedSize = srcPos;
				*uncompressedSize = destSize;
				return dest;
			}

			flags <<= 1;
		}
	}

fail:
	if(dest) free(dest);
	//~ fprintf(stderr, "couldnt decompress srcPos %06x destSize %06x\n", srcPos, destSize);
	return 0;
}

int main( int argc, char** argv ) {
	
	int extract, offset, start, end, resultsize, compressedsize;
	unsigned int size;
	
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
	
	//~ printf("extract %x offset %x end %x\n", extract, offset, end);
	
	offset = start;
	
	while(offset < end) {
		//~ fprintf(stderr, "offset %x\n", offset);
		//~ fflush(stderr);
		if((data[offset] >> 4) == 1) {
			compressedsize = (data[offset+3] << 16) | (data[offset+2] << 8) | data[offset+1];
			if(compressedsize < 0x10000) {
				compressedsize *= 2;
				resultbuffer = LZDecompress(data+offset, &compressedsize, &resultsize);
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

#include <stdio.h>
#include <stdlib.h>
#include "nitrocompression.h"

// specifically for this function, as taken from gbagfx
/*
Copyright (c) 2015 YamaArashi

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
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

/*	decompresses a file in source
	the header of the compressed file must be in the first 4 bytes of source */

unsigned char *unpackBuffer(unsigned char *source, unsigned int *resultsize, unsigned int *compressedsize) {
	//~ unsigned char *comsource = source+4;
	unsigned char *comsource = source;
	unsigned int type, mode, destinationsize;
	type = source[0] >> 4;
	mode = source[0] & 0xF;
	destinationsize = source[1] | source[2] << 8 | source[3] << 16;
	*resultsize = destinationsize;
	switch(type) {
		case 1: {
			return LZDecompress(comsource, (int *)compressedsize, (int *)resultsize);
		}
		/* disable RLE and HUFFMAN since no phoenix game uses it */
		//~ case 2:
			//~ return HUFFMANunpack(comsource, destinationsize, compressedsize, mode);
		//~ case 3:
			//~ return RLEunpack(comsource, destinationsize, compressedsize);
		
		default:
			//~ fprintf(stderr, "unknown or unsupported compression\n");
			break;
	}
	return 0;
}

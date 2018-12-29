#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "nitrocompression.h"

unsigned char *RLEunpack(unsigned char *source, unsigned int destsize, unsigned int *compsize) {
	unsigned char *destination = malloc(destsize);
	unsigned int i, flag, rle, compressed, srcpos = 0, destpos = 0;
	unsigned char workbyte;
	while( destpos < destsize ) {
		flag = source[srcpos];
		rle = flag & 0x7F;
		compressed = flag & 0x80;
		srcpos++;
		if(compressed) {
			rle+=3;
			workbyte = source[srcpos];
			srcpos++;
			for( i = 0; i < rle; i++ ) {
				destination[destpos] = workbyte;
				destpos++;
			}
		}
		else {
			rle++;
			memcpy( destination+destpos, source+srcpos, rle );
			destpos += rle;
			srcpos += rle;
		}
	}
	xprintf("compressed size was %08x bytes\n", srcpos+4);
	*compsize = srcpos+4;
	return destination;
}
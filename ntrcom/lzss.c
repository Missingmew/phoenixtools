#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "nitrocompression.h"

unsigned char *LZSSunpack(unsigned char *source, unsigned int destsize, unsigned int *compsize) {
	unsigned char *destination = malloc(destsize);
	unsigned int i, flag, comsize, uncomsize, offset, bit = 0, srcpos = 0, destpos = 0, destjmp;
	unsigned char workbyte;
	while( destpos < destsize ) {
		bit >>= 1;
		if(!bit) {
			flag = source[srcpos];
			srcpos++;
			bit = 0x80;
		}
		if(flag & bit) {
			workbyte = source[srcpos];
			srcpos++;
			comsize = workbyte >> 4;
			uncomsize = (workbyte & 0xF) << 8;
			uncomsize |= source[srcpos];
			srcpos++;
			comsize += 3;
			if(uncomsize>destpos) return 0;
			offset = destpos-uncomsize-1;
			for( i = 0; i < comsize; i++ ) {
				destjmp = destpos;
				destpos = offset++;
				workbyte = destination[destpos];
				destpos++;
				destpos = destjmp;
				destination[destpos] = workbyte;
				destpos++;
			}
		}
		else {
			destination[destpos] = source[srcpos];
			destpos++;
			srcpos++;
		}
	}
	xprintf("compressed size was %08x bytes\n", srcpos+4);
	*compsize = srcpos+4;
	return destination;
}
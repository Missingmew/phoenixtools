#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "nitrocompression.h"

unsigned char *LZXunpack(unsigned char *source, unsigned int destsize, unsigned int *compsize) {
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
			switch(workbyte>>4) {
				case 0: {
					comsize = workbyte << 4;
					workbyte = source[srcpos];
					srcpos++;
					comsize |= workbyte >> 4;
					comsize += 0x11;
					uncomsize = (workbyte & 0xF) << 8;
					workbyte = source[srcpos];
					srcpos++;
					uncomsize |= workbyte;
					break;
				}
				case 1: {
					comsize = (workbyte & 0xF) << 12;
					workbyte = source[srcpos];
					srcpos++;
					comsize |= workbyte << 4;
					workbyte = source[srcpos];
					srcpos++;
					comsize |= workbyte >> 4;
					comsize += 0x111;
					uncomsize = (workbyte & 0xF) << 8;
					workbyte = source[srcpos];
					srcpos++;
					uncomsize |= workbyte;
					break;
				}
				default: {
					comsize = (workbyte >> 4) + 1;
					uncomsize = (workbyte & 0xF) << 8;
					workbyte = source[srcpos];
					srcpos++;
					uncomsize |= workbyte;
					break;
				}
			}
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "nitrocompression.h"

unsigned char *HUFFMANunpack(unsigned char *source, unsigned int destsize, unsigned int *compsize, unsigned int mode) {
	unsigned char *destination = malloc(destsize);
	unsigned int treesize, bits = 0, workpos, mask = 0, code, nextischar, nextpos = 0, srcpos = 0, destpos = 0;
	unsigned char workbyte;
	if( mode < 4 ) return 0;
	workbyte = source[srcpos];
	srcpos++;
	treesize = workbyte;
	srcpos += (workbyte+1) << 1;
	
	while(destpos < destsize) {
		if(!(mask >>= 1)) {
			code = source[srcpos];
			srcpos += 4;
			mask = 0x80000000;
		}
		
		nextpos += ((workpos & 0x3F) + 1) << 1;
		if(!(code & mask)) {
			nextischar = workpos & 0x80;
			workpos = treesize + nextpos;
		}
		else {
			nextischar = workpos & 0x40;
			workpos = treesize + nextpos + 1;
		}
		
		if(nextischar) {
			destination[destpos] |= workpos << bits;
			if(!(bits = (bits+mode) & 7)) destpos++;
			workpos = treesize + 1;
			nextpos = 0;
		}
	}
	xprintf("compressed size was %08x bytes\n", srcpos+4);
	*compsize = srcpos+4;
	return destination;
}
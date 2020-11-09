#include <stdio.h>
#include <stdlib.h>
#include "nitrocompression.h"

/*	decompresses a file in source
	the header of the compressed file must be in the first 4 bytes of source */

unsigned char *unpackBuffer(unsigned char *source, unsigned int *resultsize, unsigned int *compressedsize) {
	unsigned int type, mode;
	type = source[0] >> 4;
	mode = source[0] & 0xF;
	xprintf("%s: type %u mode %u\n",__func__, type, mode);
	switch(type) {
		case 1: {
			if(mode == 0) return LZ10Decompress(source, (int *)compressedsize, (int *)resultsize);
			else if(mode == 1) return LZ11Decompress(source, (int *)compressedsize, (int *)resultsize);
		}
		/* disable RLE and HUFFMAN since no phoenix game uses it */
		//~ case 2:
			//~ return HUFFMANunpack(comsource, destinationsize, compressedsize, mode);
		//~ case 3:
			//~ return RLEunpack(comsource, destinationsize, compressedsize);
		
		default:
			xfprintf(stderr, "unknown or unsupported compression\n");
			break;
	}
	return 0;
}

#include <stdio.h>
#include "nitrocompression.h"

/*	decompresses a file in source
	the header of the compressed file must be in the first 4 bytes of source */

unsigned char *unpackBuffer(unsigned char *source, unsigned int *resultsize, unsigned int *compressedsize) {
	unsigned char *comsource = source+4;
	unsigned int type, mode, destinationsize;
	type = source[0] >> 4;
	mode = source[0] & 0xF;
	destinationsize = source[1] | source[2] << 8 | source[3] << 16;
	*resultsize = destinationsize;
	switch(type) {
		case 1: {
			if(mode) return LZXunpack(comsource, destinationsize, compressedsize);
			else return LZSSunpack(comsource, destinationsize, compressedsize);
		}
		/* disable RLE and HUFFMAN since no phoenix game uses it */
		case 2:
			return HUFFMANunpack(comsource, destinationsize, compressedsize, mode);
		case 3:
			return RLEunpack(comsource, destinationsize, compressedsize);
		
		default:
			fprintf(stderr, "unknown or unsupported compression\n");
			break;
	}
	return 0;
}

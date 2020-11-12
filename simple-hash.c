#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "ctrportutils.h"

#define BREAK(x) { printf(x); return 1; }


int main( int argc, char** argv ) {
	
	if ( argc < 2 )
	{
		printf( "Not enough arguments given!\nUsage: %s text\n", argv[0] );
		return 1;
	}
	uint32_t hash;
	if(strlen(argv[1]) > 512) BREAK("donut: the abuse\n");
	printf("Hashing the following string:\n%s\n", argv[1]);
	hash = hashstring(argv[1]);
	printf("Hash: %08x\n", hash);
	printf("Done.\n");
	return 0;
}

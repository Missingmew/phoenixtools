#include <stdio.h>
#include <stdint.h>
#include <string.h>

int main( int argc, char** argv ) {
	
	unsigned int foo[512] = {
		17,
		[123] = 12,
		24
	};
	
	printf("%u %u %u %u\n", foo[0], foo[1], foo[123], foo[124]);
	return 1;
	
	if ( argc < 2 )
	{
		printf( "Not enough arguments given!\nUsage: %s [infile]\n", argv[0] );
		return 1;
	}
	
	unsigned int strnlen = 1, numstr = 0;
	uint32_t off;
	uint8_t ver;
	char string[512] = {0};
	
	FILE *f = NULL;
	
	if( !(f = fopen( argv[1], "rb" ))) {
		printf("Couldnt open file %s as source\n", argv[1]);
		return 1;
	}
	fseek(f, 4, SEEK_SET);
	fread(&ver, 1, 1, f);
	if(ver<0x13) fseek(f, 0xF4, SEEK_SET);
	else fseek(f, 0x104, SEEK_SET);
	fread(&off, 4, 1, f);
	fseek(f, off, SEEK_SET);
	while(strnlen) {
		fgets(string, 512, f);
		printf("%02d %02x - %s\n", numstr, numstr, string);
		numstr++;
		strnlen = strlen(string);
		off+=strnlen+1;
		fseek(f, off, SEEK_SET);
	}
	fclose(f);
	printf("total strings: %d\n", numstr-1);
	return 0;
}

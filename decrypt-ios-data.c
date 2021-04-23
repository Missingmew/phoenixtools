#include <stdio.h>
#include <stdlib.h>

void decrypt(unsigned char *buf, int initial_off, int size) {
    char localstring[] = "FaNtA-500mL_mElOn=SoDa";
    int localindex = initial_off % 22;
    for(unsigned i = 0; i < size; i++) {
        buf[i] ^= localstring[localindex++];
        if(localindex > 21) localindex = 0;
    }
}

int main(int argc, char **argv) {
	FILE *in, *out;
	unsigned insize;
	unsigned char *data;
	
	if(argc < 3) {
		printf("usage: %s infile outfile\n", argv[0]);
		return 1;
	}
	
	if(!(in = fopen(argv[1], "rb"))) {
		printf("couldnt open %s for reading\n", argv[1]);
		return 1;
	}
	
	if(!(out = fopen(argv[2], "wb"))) {
		printf("couldnt open %s for writing\n", argv[2]);
		return 1;
	}
	
	fseek(in, 0, SEEK_END);
	insize = ftell(in);
	fseek(in, 0, SEEK_SET);
	
	data = malloc(insize);
	fread(data, insize, 1, in);
	fclose(in);
	
	decrypt(data, 0, insize);
	
	fwrite(data, insize, 1, out);
	fclose(out);
	printf("done\n");
	return 0;
}

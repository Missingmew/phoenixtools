#include <stdio.h>
#include <stdlib.h> 
#include "rg_etc1.hh"

//requires rg_etc1.cpp and rg_etc1.h from https://code.google.com/p/rg-etc1/source/browse/#svn%2Ftrunk

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

u64 switchEndianness(u64 v)
{
	u64 ret;
	u8* retA=(u8*)&ret;
	u8* vA=(u8*)&v;

	retA[0]=vA[7];
	retA[1]=vA[6];
	retA[2]=vA[5];
	retA[3]=vA[4];
	retA[4]=vA[3];
	retA[5]=vA[2];
	retA[6]=vA[1];
	retA[7]=vA[0];

	return ret;
}


//basically just runs decompression while accounting for endianness and tiling
int main(int argc, char* argv[])
{
	int w, h;
	if(argc<4) {
		printf("notenufargs\n");
		return -1;
	}

	FILE* f=fopen(argv[1],"rb");
	sscanf(argv[2],"%d",&w);
	sscanf(argv[3],"%d",&h);
	u32* buffer=(u32*)malloc(sizeof(u32)*w*h);
	if(!f) {
		printf("cnatopnfiel\n");
		return -1;
	}

	int i, j;
	for(i=0;i<w;i+=8)
	{
		for(j=0;j<h;j+=8)
		{
			u64 block;
			u32 pixels[4*4];
			int x, y;
			u32* ptr;

			fread(&block, 8, 1, f);
			block=switchEndianness(block);
			rg_etc1::unpack_etc1_block(&block, pixels, false);
			ptr=pixels;
			for(x=i;x<i+4;x++)
				for(y=j;y<j+4;y++)
					buffer[y+x*h]=*(ptr++);

			fread(&block, 8, 1, f);
			block=switchEndianness(block);
			rg_etc1::unpack_etc1_block(&block, pixels, false);
			ptr=pixels;
			for(x=i;x<i+4;x++)
				for(y=j+4;y<j+8;y++)
					buffer[y+x*h]=*(ptr++);

			fread(&block, 8, 1, f);
			block=switchEndianness(block);
			rg_etc1::unpack_etc1_block(&block, pixels, false);
			ptr=pixels;
			for(x=i+4;x<i+8;x++)
				for(y=j;y<j+4;y++)
					buffer[y+x*h]=*(ptr++);

			fread(&block, 8, 1, f);
			block=switchEndianness(block);
			rg_etc1::unpack_etc1_block(&block, pixels, false);
			ptr=pixels;
			for(x=i+4;x<i+8;x++)
				for(y=j+4;y<j+8;y++)
					buffer[y+x*h]=*(ptr++);
		}
	}
	fclose(f);

	char out[1024];
	sprintf(out,"%s.data",argv[1]);
	FILE* fout=fopen(out,"wb");
		fwrite(buffer, 4, w*h, fout);
	fclose(fout);

	free(buffer);
	return 0;
}
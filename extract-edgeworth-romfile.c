#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "ntrcom/nitrocompression.h"

typedef struct {
	uint32_t offset;
	uint32_t size;
}arcEntry;

int main( int argc, char** argv ) {
	
	if ( argc < 2 )
	{
		printf( "Not enough arguments given!\nUsage: %s [file]\nFiles will be extracted in current working directory.\n", argv[0] );
		return 1;
	}
	
	uint32_t listsize;
	unsigned int i, numFiles, resultsize, compressedsize;
	char outputname[32] = { 0 };
	unsigned char *workbuf = NULL, *resultbuf = NULL;
	arcEntry *filelist = NULL;
	FILE *f, *o;
	
	if( !(f = fopen( argv[1], "rb" ))) {
		printf("Couldnt open file %s\n", argv[1]);
		return 1;
	}
	
	fread( &listsize, 4, 1, f );
	numFiles = listsize/4-1;
	printf("listsize is %08x, numfiles is %08d\n", listsize, numFiles);
	filelist = malloc(sizeof(arcEntry)*numFiles);
	for( i = 0; i < numFiles; i++ ) {
		printf("file %08d - ", i);
		fseek( f, 8+i*4, SEEK_SET );
		fread( &filelist[i].offset, 4, 1, f );
		printf("offset %08x - ", filelist[i].offset);
		fseek( f, filelist[i].offset, SEEK_SET );
		fread( &filelist[i].size, 4, 1, f );
		printf("size %08x\n", filelist[i].size);
		filelist[i].offset += 4;
	}
	
	for( i = 0; i < numFiles; i++ ) {
		printf("Current file %u offset: %08x - Current file size %08x\n", i, filelist[i].offset, filelist[i].size);
		fflush(stdout);
		
		if(filelist[i].size) {
			workbuf = malloc(filelist[i].size);
			fseek( f, filelist[i].offset, SEEK_SET );
			fread( workbuf, filelist[i].size, 1, f );
			
			compressedsize = filelist[i].size;
			
			resultbuf = unpackBuffer( workbuf, &resultsize, &compressedsize );
			if(!resultbuf) {
				//~ printf("file %08d is uncompressed!\n", i);
				resultsize = 0;
			}
			if(resultsize < compressedsize) {
				if(resultbuf) printf("unpak less then pak %08x vs %08x\n", resultsize, compressedsize);
				if(resultsize) free(resultbuf);
				resultsize = 0;
			}
		}
		else {
			resultsize = 0;
		}
		if(resultsize) {
			/* NITRO common filetypes */
			if( !strncmp( (char *)resultbuf, "BCA0", 4 )) snprintf( outputname, 32, "%08d-%08x.nsbca", i, filelist[i].offset );
			else if( !strncmp( (char *)resultbuf, "BMD0", 4 )) snprintf( outputname, 32, "%08d-%08x.nsbmd", i, filelist[i].offset );
			else if( !strncmp( (char *)resultbuf, "BTX0", 4 )) snprintf( outputname, 32, "%08d-%08x.nsbtx", i, filelist[i].offset );
			else if( !strncmp( (char *)resultbuf, "BTA0", 4 )) snprintf( outputname, 32, "%08d-%08x.nsbta", i, filelist[i].offset );
			else if( !strncmp( (char *)resultbuf, "RNAN", 4 )) snprintf( outputname, 32, "%08d-%08x.nanr", i, filelist[i].offset );
			else if( !strncmp( (char *)resultbuf, "RECN", 4 )) snprintf( outputname, 32, "%08d-%08x.ncer", i, filelist[i].offset );
			else if( !strncmp( (char *)resultbuf, "RGCN", 4 )) snprintf( outputname, 32, "%08d-%08x.ncgr", i, filelist[i].offset );
			else if( !strncmp( (char *)resultbuf, "RLCN", 4 )) snprintf( outputname, 32, "%08d-%08x.nclr", i, filelist[i].offset );
			else if( !strncmp( (char *)resultbuf, "RCMN", 4 )) snprintf( outputname, 32, "%08d-%08x.nmcr", i, filelist[i].offset );
			else if( !strncmp( (char *)resultbuf, "RCSN", 4 )) snprintf( outputname, 32, "%08d-%08x.nscr", i, filelist[i].offset );
			else if( !strncmp( (char *)resultbuf, "NARC", 4 )) snprintf( outputname, 32, "%08d-%08x.narc", i, filelist[i].offset );
			// else if( resultbuf[0] == 0x42 && resultbuf[1] == 0x4D && resultbuf[2] == 0x44 && resultbuf[3] == 0x30 ) snprintf( outputname, 32, "%08d-%08x.nsbmd", i, filelist[i].offset );
			// else if( resultbuf[0] == 0x42 && resultbuf[1] == 0x43 && resultbuf[2] == 0x41 && resultbuf[3] == 0x30 ) snprintf( outputname, 32, "%08d-%08x.nsbca", i, filelist[i].offset );
			else if( resultsize == 32 ) snprintf( outputname, 32, "%08d-%08x.pal4", i, filelist[i].offset );
			else if( resultsize == 512 ) snprintf( outputname, 32, "%08d-%08x.pal8", i, filelist[i].offset );
			else if( resultsize % 0x20 == 0 ) snprintf( outputname, 32, "%08d-%08x-img.bin", i, filelist[i].offset );
			else snprintf( outputname, 32, "%08d-%08x-unk.bin", i, filelist[i].offset );
		}
		else {
			/* NITRO common filetypes */
			if( filelist[i].size == 0 ) snprintf( outputname, 32, "%08d-%08x.null", i, filelist[i].offset );
			else if( !strncmp( (char *)workbuf, "BCA0", 4 )) snprintf( outputname, 32, "%08d-%08x.nsbca", i, filelist[i].offset );
			else if( !strncmp( (char *)workbuf, "BMD0", 4 )) snprintf( outputname, 32, "%08d-%08x.nsbmd", i, filelist[i].offset );
			else if( !strncmp( (char *)workbuf, "BTX0", 4 )) snprintf( outputname, 32, "%08d-%08x.nsbtx", i, filelist[i].offset );
			else if( !strncmp( (char *)workbuf, "BTA0", 4 )) snprintf( outputname, 32, "%08d-%08x.nsbta", i, filelist[i].offset );
			else if( !strncmp( (char *)workbuf, "RNAN", 4 )) snprintf( outputname, 32, "%08d-%08x.nanr", i, filelist[i].offset );
			else if( !strncmp( (char *)workbuf, "RECN", 4 )) snprintf( outputname, 32, "%08d-%08x.ncer", i, filelist[i].offset );
			else if( !strncmp( (char *)workbuf, "RGCN", 4 )) snprintf( outputname, 32, "%08d-%08x.ncgr", i, filelist[i].offset );
			else if( !strncmp( (char *)workbuf, "RLCN", 4 )) snprintf( outputname, 32, "%08d-%08x.nclr", i, filelist[i].offset );
			else if( !strncmp( (char *)workbuf, "RCMN", 4 )) snprintf( outputname, 32, "%08d-%08x.nmcr", i, filelist[i].offset );
			else if( !strncmp( (char *)workbuf, "RCSN", 4 )) snprintf( outputname, 32, "%08d-%08x.nscr", i, filelist[i].offset );
			else if( !strncmp( (char *)workbuf, "NARC", 4 )) snprintf( outputname, 32, "%08d-%08x.narc", i, filelist[i].offset );
			// else if( workbuf[0] == 0x42 && workbuf[1] == 0x4D && workbuf[2] == 0x44 && workbuf[3] == 0x30 ) snprintf( outputname, 32, "%08d-%08x.nsbmd", i, filelist[i].offset );
			// else if( workbuf[0] == 0x42 && workbuf[1] == 0x43 && workbuf[2] == 0x41 && workbuf[3] == 0x30 ) snprintf( outputname, 32, "%08d-%08x.nsbca", i, filelist[i].offset );
			else if( filelist[i].size == 32 ) snprintf( outputname, 32, "%08d-%08x.pal4", i, filelist[i].offset );
			else if( filelist[i].size == 512 ) snprintf( outputname, 32, "%08d-%08x.pal8", i, filelist[i].offset );
			else if( filelist[i].size % 0x20 == 0 ) snprintf( outputname, 32, "%08d-%08x-img.bin", i, filelist[i].offset );
			else snprintf( outputname, 32, "%08d-%08x-unk.bin", i, filelist[i].offset );
		}
		
		if( !(o = fopen( outputname, "wb" ))) {
				printf("Couldnt open file %s\n", outputname);
				return 1;
		}
		
		if(!resultsize) fwrite( workbuf, filelist[i].size, 1, o );
		else fwrite( resultbuf, resultsize, 1, o );
		fclose(o);
		if(filelist[i].size) free( workbuf );
		
		
		if(resultsize) free( resultbuf );
	}
	free(filelist);
	printf("Done.\n");
	fclose(f);
	return 0;
	
}

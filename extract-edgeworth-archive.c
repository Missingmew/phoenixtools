#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "ntrcom/nitrocompression.h"

typedef struct {
	uint32_t offset;
	uint32_t size;
	uint32_t compressed;
}__attribute__((packed)) arcEntry;

int main( int argc, char** argv ) {
	
	if ( argc < 3 )
	{
		printf( "Not enough arguments given!\nUsage: %s [file] [type]\ntype 0: big archives found in root\ntype 1: archives found inside archives of type 0\nFiles will be extracted in current working directory.\n", argv[0] );
		return 1;
	}
	
	uint32_t listsize;
	unsigned int i, type, numFiles, resultsize, compressedsize;
	char outputname[32] = { 0 };
	unsigned char *workbuf = NULL, *resultbuf = NULL;
	arcEntry *filelist = NULL;
	FILE *f, *o;
	type = strtoul(argv[2], NULL, 10);
	
	if( !(f = fopen( argv[1], "rb" ))) {
		printf("Couldnt open file %s\n", argv[1]);
		return 1;
	}
	
	fread( &listsize, 4, 1, f );
	if(type) {
		numFiles = listsize/4;
		printf("listsize is %08x, numfiles is %08d\n", listsize, numFiles);
		filelist = malloc(sizeof(arcEntry)*numFiles);
		for( i = 0; i < numFiles; i++ ) {
			fseek( f, 4*i, SEEK_SET );
			fread( &filelist[i], 4, 1, f );
			filelist[i].compressed = 0;
		}
		fseek( f, 0, SEEK_END );
		filelist[numFiles-1].size = ftell(f) - filelist[numFiles-1].offset;
		for( i = 0; i < numFiles-1; i++ ) filelist[i].size = filelist[i+1].offset - filelist[i].offset;
		printf("file %08d - offset %08x - size %08x - is compressed %d\n", i, filelist[i].offset, filelist[i].size, filelist[i].compressed);
	}
	else {
		numFiles = listsize/8-1;
		printf("listsize is %08x, numfiles is %08d\n", listsize, numFiles);
		filelist = malloc(sizeof(arcEntry)*numFiles);
		for( i = 0; i < numFiles; i++ ) {
			fseek( f, 8*i, SEEK_SET );
			fread( &filelist[i], 8, 1, f );
			if( filelist[i].size & (1<<31)) {
				filelist[i].size &= 0x7FFFFFFF;
				filelist[i].compressed = 1;
			}
			else filelist[i].compressed = 0;
			printf("file %08d - offset %08x - size %08x - is compressed %d\n", i, filelist[i].offset, filelist[i].size, filelist[i].compressed);
		}
	}
	
	for( i = 0; i < numFiles; i++ ) {
		printf("Current file offset: %08x - Current file size %08x\n", filelist[i].offset, filelist[i].size);
		fflush(stdout);
		workbuf = malloc(filelist[i].size);
		fseek( f, filelist[i].offset, SEEK_SET );
		fread( workbuf, filelist[i].size, 1, f );
		
		if( filelist[i].compressed ) {
			compressedsize = filelist[i].size;
			resultbuf = unpackBuffer( workbuf, &resultsize, &compressedsize );
			if(!resultbuf) {
				printf("file %08d is uncompressed!\n", i);
				resultsize = 0;
			}
			if(resultsize < compressedsize) {
				resultsize = 0;
				free(resultbuf);
			}
		}
		else resultsize = 0;
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
			
			/* other types */
			else if( !strncmp( (char *)resultbuf, "MDAT", 4 )) snprintf( outputname, 32, "%08d-%08x.MDAT", i, filelist[i].offset );
			else if( !strncmp( (char *)resultbuf+0xC, "RECN", 4 )) snprintf( outputname, 32, "%08d-%08x.gk2spak", i, filelist[i].offset );
			// else if( !strncmp( (char *)resultbuf, "", 4 )) snprintf( outputname, 32, "%08d-%08x.", i, filelist[i].offset );
			else snprintf( outputname, 32, "%08d-%08x.bin", i, filelist[i].offset );
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
			
			/* other types */
			else if( !strncmp( (char *)workbuf, "MDAT", 4 )) snprintf( outputname, 32, "%08d-%08x.MDAT", i, filelist[i].offset );
			else if( !strncmp( (char *)workbuf+0xC, "RECN", 4 )) snprintf( outputname, 32, "%08d-%08x.gk2spak", i, filelist[i].offset );
			// else if( !strncmp( (char *)workbuf, "", 4 )) snprintf( outputname, 32, "%08d-%08x.", i, filelist[i].offset );
			else snprintf( outputname, 32, "%08d-%08x.bin", i, filelist[i].offset );
		}
		
		if( !(o = fopen( outputname, "wb" ))) {
				printf("Couldnt open file %s\n", outputname);
				return 1;
		}
		
		if(!resultsize) fwrite( workbuf, filelist[i].size, 1, o );
		else fwrite( resultbuf, resultsize, 1, o );
		fclose(o);
		free( workbuf );
		
		
		if(resultsize) free( resultbuf );
	}
	printf("Done.\n");
	fclose(f);
	return 0;
	
}

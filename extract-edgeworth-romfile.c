#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "ntrcom/nitrocompression.h"

typedef struct {
	uint32_t offset;
	uint32_t size;
}arcEntry;

typedef enum {
	LINEUSE,
	LINEEMPTY
} linetype;

int main( int argc, char** argv ) {
	
	if ( argc < 3 )
	{
		printf( "Not enough arguments given!\nUsage: %s [file] [list]\nFiles will be extracted in current working directory.\n", argv[0] );
		return 1;
	}
	
	uint32_t listsize;
	unsigned int i, numFiles, resultsize, compressedsize;
	char outputname[256] = { 0 }, mapline[256] = {0};
	unsigned char *workbuf = NULL, *resultbuf = NULL;
	linetype curline;
	arcEntry *filelist = NULL;
	FILE *f, *t, *o;
	
	if( !(f = fopen( argv[1], "rb" ))) {
		printf("Couldnt open file %s\n", argv[1]);
		return 1;
	}
	
	if( !(t = fopen( argv[2], "r" ))) {
		printf("Couldnt open file %s\n", argv[2]);
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
		fgets(mapline, 256, t);
		while(!(strncmp(mapline, "//", 2)) && !feof(t)) fgets(mapline, 256, t);
		if(feof(t)) curline = LINEEMPTY;
		else if(strlen(mapline) < 3) curline = LINEEMPTY;
		else curline = LINEUSE;
		
		
		while(strcspn(mapline, "\r\n") < strlen(mapline)) mapline[strcspn(mapline,"\r\n")] = 0;
		
		
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
		
		/* NITRO common filetypes */
		if(curline == LINEUSE) {
			snprintf(outputname, 256, "%04d-%.250s", i, mapline);
		}
		else {
			if( filelist[i].size == 0 ) snprintf( outputname, 256, "%04d-%08x.null", i, filelist[i].offset );
			else if( !strncmp( (char *)(resultsize ? resultbuf : workbuf), "BCA0", 4 )) snprintf( outputname, 256, "%04d-%08x.nsbca", i, filelist[i].offset );
			else if( !strncmp( (char *)(resultsize ? resultbuf : workbuf), "BMD0", 4 )) snprintf( outputname, 256, "%04d-%08x.nsbmd", i, filelist[i].offset );
			else if( !strncmp( (char *)(resultsize ? resultbuf : workbuf), "BTX0", 4 )) snprintf( outputname, 256, "%04d-%08x.nsbtx", i, filelist[i].offset );
			else if( !strncmp( (char *)(resultsize ? resultbuf : workbuf), "BTA0", 4 )) snprintf( outputname, 256, "%04d-%08x.nsbta", i, filelist[i].offset );
			else if( !strncmp( (char *)(resultsize ? resultbuf : workbuf), "RNAN", 4 )) snprintf( outputname, 256, "%04d-%08x.nanr", i, filelist[i].offset );
			else if( !strncmp( (char *)(resultsize ? resultbuf : workbuf), "RECN", 4 )) snprintf( outputname, 256, "%04d-%08x.ncer", i, filelist[i].offset );
			else if( !strncmp( (char *)(resultsize ? resultbuf : workbuf), "RGCN", 4 )) snprintf( outputname, 256, "%04d-%08x.ncgr", i, filelist[i].offset );
			else if( !strncmp( (char *)(resultsize ? resultbuf : workbuf), "RLCN", 4 )) snprintf( outputname, 256, "%04d-%08x.nclr", i, filelist[i].offset );
			else if( !strncmp( (char *)(resultsize ? resultbuf : workbuf), "RCMN", 4 )) snprintf( outputname, 256, "%04d-%08x.nmcr", i, filelist[i].offset );
			else if( !strncmp( (char *)(resultsize ? resultbuf : workbuf), "RCSN", 4 )) snprintf( outputname, 256, "%04d-%08x.nscr", i, filelist[i].offset );
			else if( !strncmp( (char *)(resultsize ? resultbuf : workbuf), "NARC", 4 )) snprintf( outputname, 256, "%04d-%08x.narc", i, filelist[i].offset );
			else if( (resultsize ? resultsize : filelist[i].size) == 32 ) snprintf( outputname, 32, "%04d-%08x.pal4", i, filelist[i].offset );
			else if( (resultsize ? resultsize : filelist[i].size) == 512 ) snprintf( outputname, 32, "%04d-%08x.pal8", i, filelist[i].offset );
			else if( (resultsize ? resultsize : filelist[i].size) % 0x20 == 0 ) snprintf( outputname, 32, "%04d-%08x-img.bin", i, filelist[i].offset );
			else snprintf( outputname, 32, "%04d-%08x-unk.bin", i, filelist[i].offset );
		}
		
		if( !(o = fopen( outputname, "wb" ))) {
				printf("Couldnt open file %s\n", outputname);
				//~ return 1;
			continue;
		}
		
		if(!resultsize) fwrite( workbuf, filelist[i].size, 1, o );
		else fwrite( resultbuf, resultsize, 1, o );
		fclose(o);
		if(filelist[i].size) free( workbuf );
		
		
		if(resultsize) free( resultbuf );
	}
	free(filelist);
	printf("Done.\n");
	fclose(t);
	fclose(f);
	return 0;
	
}

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "ntrcom/nitrocompression.h"

#ifdef _WIN32
#include <direct.h>
#define createDirectory(dirname) mkdir(dirname)
#else
#include <sys/stat.h>
#include <sys/types.h>
#define createDirectory(dirname) mkdir(dirname, 0777)
#endif

typedef struct {
	uint32_t offset;
	uint32_t size;
} arcEntry;

typedef enum {
	LINEUSE,
	LINEEMPTY
} linetype;

void makepath(char *path) {
		//~ printf("making directory %s\n", path);
	FILE *tmp = NULL;
	if((tmp = fopen(path, "wb"))) {
		remove(path);
		fclose(tmp);
		return;
	}
	//~ printf("bar\n");
	char bakpath[2048] = {0}, curpath[2048] = {0};
	char *token = NULL;
	strcpy(bakpath, path);
	token = strtok(bakpath, "/");
	strcpy(curpath, token);
	
	while(token) {
		createDirectory(curpath);
		//~ printf("creating directory %s\n", curpath);
		strcat(curpath, "/");
		token = strtok(NULL, "/");
		strcat(curpath, token);
		if((tmp = fopen(path, "wb"))) break;
	}
	fclose(tmp);
	remove(path);
	fflush(stdout);
	return;
}

int main( int argc, char** argv ) {
	
	if ( argc < 2 )
	{
		printf( "Not enough arguments given!\nUsage: %s [file] [optional list]\nFiles will be extracted in current working directory.\n", argv[0] );
		return 1;
	}
	
	uint32_t listsize;
	unsigned int i, numFiles, resultsize, compressedsize;
	char outputfile[512], outputname[256], mapline[256];
	unsigned char *workbuf = NULL, *resultbuf = NULL;
	linetype curline;
	arcEntry *filelist = NULL;
	FILE *f, *t = NULL, *o;
	
	if( !(f = fopen( argv[1], "rb" ))) {
		printf("Couldnt open file %s\n", argv[1]);
		return 1;
	}
	
	if(argc == 3) {
		if( !(t = fopen( argv[2], "r" ))) {
			printf("Couldnt open file %s\n", argv[2]);
			return 1;
		}
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
	
	curline = LINEEMPTY;
	
	for( i = 0; i < numFiles; i++ ) {
		//~ printf("Current file %u offset: %08x - Current file size %08x\n", i, filelist[i].offset, filelist[i].size);
		if(t) {
			fgets(mapline, 256, t);
			while(!(strncmp(mapline, "//", 2)) && !feof(t)) fgets(mapline, 256, t);
			if(feof(t)) curline = LINEEMPTY;
			else if(strlen(mapline) < 3) curline = LINEEMPTY;
			else curline = LINEUSE;
		}
		
		
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
		
		if(curline == LINEUSE) {
			sprintf(outputname, "%.255s", mapline);
		}
		else {
			sprintf(outputname, "%04d-%08x", i, filelist[i].offset);
		}
		
		/* NITRO common filetypes */
		if( filelist[i].size == 0 ) sprintf(outputfile, "%s.null", outputname);
		else if( !strncmp( (char *)(resultsize ? resultbuf : workbuf), "BCA0", 4 )) sprintf(outputfile, "%s.nsbca", outputname);
		else if( !strncmp( (char *)(resultsize ? resultbuf : workbuf), "BMD0", 4 )) sprintf(outputfile, "%s.nsbmd", outputname);
		else if( !strncmp( (char *)(resultsize ? resultbuf : workbuf), "BTX0", 4 )) sprintf(outputfile, "%s.nsbtx", outputname);
		else if( !strncmp( (char *)(resultsize ? resultbuf : workbuf), "BTA0", 4 )) sprintf(outputfile, "%s.nsbta", outputname);
		else if( !strncmp( (char *)(resultsize ? resultbuf : workbuf), "RNAN", 4 )) sprintf(outputfile, "%s.nanr", outputname);
		else if( !strncmp( (char *)(resultsize ? resultbuf : workbuf), "RECN", 4 )) sprintf(outputfile, "%s.ncer", outputname);
		else if( !strncmp( (char *)(resultsize ? resultbuf : workbuf), "RGCN", 4 )) sprintf(outputfile, "%s.ncgr", outputname);
		else if( !strncmp( (char *)(resultsize ? resultbuf : workbuf), "RLCN", 4 )) sprintf(outputfile, "%s.nclr", outputname);
		else if( !strncmp( (char *)(resultsize ? resultbuf : workbuf), "RCMN", 4 )) sprintf(outputfile, "%s.nmcr", outputname);
		else if( !strncmp( (char *)(resultsize ? resultbuf : workbuf), "RCSN", 4 )) sprintf(outputfile, "%s.nscr", outputname);
		else if( !strncmp( (char *)(resultsize ? resultbuf : workbuf), "NARC", 4 )) sprintf(outputfile, "%s.narc", outputname);
		else if( (resultsize ? resultsize : filelist[i].size) == 32 ) sprintf(outputfile, "%s.pal4", outputname);
		else if( (resultsize ? resultsize : filelist[i].size) == 512 ) sprintf(outputfile, "%s.pal8", outputname);
		else if( (resultsize ? resultsize : filelist[i].size) % 0x20 == 0 ) sprintf(outputfile, "%s-img.bin", outputname);
		else sprintf(outputfile, "%s-unk.bin", outputname);
		
		makepath(outputfile);
		
		if( !(o = fopen( outputfile, "wb" ))) {
			printf("Couldnt open file %s\n", outputname);
			//~ return 1;
			continue;
		}
		
		printf("writing to %s\n", outputfile);
		
		if(!resultsize) fwrite( workbuf, filelist[i].size, 1, o );
		else fwrite( resultbuf, resultsize, 1, o );
		fclose(o);
		if(filelist[i].size) free( workbuf );
		
		
		if(resultsize) free( resultbuf );
	}
	free(filelist);
	printf("Done.\n");
	if(t) fclose(t);
	fclose(f);
	return 0;
	
}

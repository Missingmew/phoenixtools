#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include "ntrcom/nitrocompression.h"
#include "ctrportutils.h"

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
	uint32_t null;
	uint32_t unpaksize;
	uint32_t paksize;
	uint32_t namecrc;
}__attribute__((packed)) pakEntry;

typedef struct {
	uint32_t namecrc;
	char name[256];
	unsigned used;
} dictEntry;

void createFileName( char *name, unsigned char *buffer, unsigned int i ) {
	// if( buffer[0] == 0x42 && buffer[1] == 0x43 && buffer[2] == 0x48 && buffer[3] == 0 ) snprintf( name, 32, "%08d-%08x.bch", i, i ); // BCH
	if( !strncmp( (char *)buffer, "BCH", 3 )) snprintf( name, 32, "%08d-%08x.bch", i, i ); // BCH
	else if( !strncmp( (char *)buffer, "FFNT", 4 )) snprintf( name, 32, "%08d-%08x.bcfnt", i, i ); // FFNT
	else if( !strncmp( (char *)buffer, ".ans", 4 )) snprintf( name, 32, "%08d-%08x.ans", i, i ); // .ans
	else if( !strncmp( (char *)buffer, "locm", 4 )) snprintf( name, 32, "%08d-%08x.locm", i, i ); // .locm
	else if ((i == 58) || (i == 6786) || (i == 9622)) snprintf( name, 32, "%08d-%08x.pxs", i, i ); // phoenix scripts as found in the NDS games
	else snprintf( name, 32, "%08d-%08x.bin", i, i );
	return;
}

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

unsigned findname(char *dest, dictEntry *dic, unsigned dicsize, uint32_t namecrc) {
	if(!dic) return 0;
	for(unsigned i = 0; i < dicsize; i++) {
		if(dic[i].namecrc == namecrc) {
			strcpy(dest, dic[i].name);
			dic[i].used = 1;
			return 1;
		}
	}
	return 0;
}

int main( int argc, char** argv ) {
	
	if ( argc < 3 )
	{
		printf( "Not enough arguments given!\nUsage: %s pack.inc pack.dat [dictionary]\nFiles will be extracted in current working directory.\n", argv[0] );
		return 1;
	}
	
	unsigned int resultsize, compressedsize, i, numentries, dicsize;
	char outputname[256];
	unsigned char *workbuffer = NULL, *resultbuffer = NULL;
	pakEntry *paklist;
	dictEntry *dictionary = NULL;
	
	FILE *inc, *pak, *o;
	
	if( !(inc = fopen( argv[1], "rb" ))) {
		printf("Couldnt open file %s\n", argv[1]);
		return 1;
	}
	if( !(pak = fopen( argv[2], "rb" ))) {
		printf("Couldnt open file %s\n", argv[2]);
		return 1;
	}
	
	fseek(inc, 0, SEEK_END);
	numentries = ftell(inc);
	fseek(inc, 0, SEEK_SET);
	numentries /= sizeof(pakEntry);
	
	paklist = malloc(numentries*sizeof(pakEntry));
	fread(paklist, numentries*sizeof(pakEntry), 1, inc);
	fclose(inc);
	
	if(argc == 4) {
		unsigned line = 0, curalloc = 0, curcount = 0;
		int numitems;
		char scanline[512], path[256];
		if( !(inc = fopen( argv[3], "r" ))) {
			printf("Couldnt open file %s\n", argv[3]);
			return 1;
		}
		
		while(!feof(inc)) {
			line++;
			fgets(scanline, sizeof(scanline), inc);
			numitems = sscanf(scanline, "\"%255[^\"]\"", path);
			if(numitems == 1) {
				// assume that anything thats not starting with alphanumeric characters or isnt in double quotes is an uninteresting string
				if(scanline[0] != '\"' && !isalnum(scanline[0])) continue;
				if(curcount == curalloc) {
					curalloc += 100;
					dictionary = realloc(dictionary, curalloc*sizeof(dictEntry));
				}
				dictionary[curcount].namecrc = hashstring(path);
				memcpy(dictionary[curcount].name, path, 256);
				dictionary[curcount].used = 0;
				
				//~ printf("line %u calculated chksum %08x for path %s\n", line, dictionary[curcount].namecrc, dictionary[curcount].name);
				curcount++;
			}
		}
		
		dicsize = curcount;
		fclose(inc);
		printf("added a total of %u entries to dictionary\n", curcount);
	}
	
	for(i = 0; i < numentries; i++) {
		//~ printf("File %08d - Offset: %08x - Compressed %08x - Uncompressed %08x - NameCRC %08x\n", i, paklist[i].offset, paklist[i].paksize, paklist[i].unpaksize, paklist[i].namecrc);
		compressedsize = paklist[i].paksize;
		
		fseek( pak, paklist[i].offset, SEEK_SET );
		if(paklist[i].unpaksize) {
			// compressed file
			workbuffer = malloc( paklist[i].paksize );
			fread( workbuffer, paklist[i].paksize, 1, pak );
			resultbuffer = unpackBuffer( workbuffer, &resultsize, &compressedsize );
			if( !resultbuffer ) {
				printf("Failed to decompress file!\n");
				return 1;
			}
			if( !(resultsize == paklist[i].unpaksize)) printf( "Uncompressed buffer size does not match recorded buffer size (%08x - %08x)!\n", resultsize, paklist[i].unpaksize);
			free(workbuffer);
		}
		else {
			// uncompressed file
			resultbuffer = malloc(paklist[i].paksize);
			fread(resultbuffer, paklist[i].paksize, 1, pak);
			resultsize = paklist[i].paksize;
		}		
		if(!findname(outputname, dictionary, dicsize, paklist[i].namecrc)) {
			printf("failed to find a name for file %u, namecrc %08x\n", i, paklist[i].namecrc);
			createFileName( outputname, resultbuffer, i );
		}
		makepath(outputname);
		if( !(o = fopen( outputname, "wb" ))) {
			printf("Couldnt open file %s\n", outputname);
			return 1;
		}
		fwrite( resultbuffer, resultsize, 1, o );
		fclose(o);
		free(resultbuffer);
	}
	
	for(i = 0; i < dicsize; i++) {
		if(!dictionary[i].used) {
			printf("dictionary entry '%s' with crc %08x wasnt used\n", dictionary[i].name, dictionary[i].namecrc);
		}
	}
	
	free(dictionary);
	free(paklist);
	fclose(pak);
	printf("Done.\n");
	return 0;
}

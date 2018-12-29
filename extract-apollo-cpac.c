#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "ntrcom/nitrocompression.h"

#define BREAK(x) { printf(x); return 1; }

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
}__attribute__((packed)) cpacEntry;

typedef struct {
	uint32_t size;
	uint32_t numSections;
}__attribute__((packed)) subarcHeader;

typedef struct {
	char magic[4];
	uint32_t offset;
}__attribute__((packed)) subarcSection;

typedef struct {
	uint32_t offset;
	uint32_t size;
}__attribute__((packed)) subarcEntry;

int main( int argc, char** argv ) {
	
	if ( argc < 2 )
	{
		printf( "Not enough arguments given!\nUsage: %s [cpac*.bin]\nDirectory structure will be created in current directory\n", argv[0] );
		return 1;
	}
	
	cpacEntry filelist[8];
	subarcHeader subarchead;
	subarcSection sections[2];
	subarcEntry *entries;
	unsigned int i, j, numSubarcs, numFiles, resultsize, compressedsize;
	FILE *f, *o;
	char outputname[64] = {0};
	unsigned char *workbuf = NULL, *resultbuf = NULL;
	
	if( !(f = fopen( argv[1], "rb" ))) {
		printf("Couldnt open file %s\n", argv[1]);
		return 1;
	}
	
	fread( &filelist[0], sizeof(cpacEntry), 1, f );
	i = 1;
	while(ftell(f) < filelist[0].offset) {
		fread( &filelist[i], sizeof(cpacEntry), 1, f );
		i++;
	}
	numSubarcs = i;
	 /* for dumping subarcs
	for( i = 0; i < numSubarcs; i++ ) {
		snprintf( outputname, 64, "subarc-%02d.bin", i );
		fseek( f, filelist[i].offset, SEEK_SET );
		workbuf = malloc(filelist[i].size);
		fread( workbuf, filelist[i].size, 1, f );
		if( !(o = fopen( outputname, "wb" ))) {
			printf("Couldnt open file %s\n", outputname);
			return 1;
		}
		fwrite( workbuf, filelist[i].size, 1, o );
		fclose(o);
		free(workbuf);
	} */
	
	for( i = 0; i < numSubarcs; i++ ) {
		fseek(f, filelist[i].offset, SEEK_SET);
		fread( &subarchead, sizeof(subarcHeader), 1, f );
		for( j = 0; j < subarchead.numSections; j++ ) fread( &sections[j], sizeof(subarcSection), 1, f );
		if( sections[0].magic[3] != 0x42 ) {
			// BREAK("YEKP table isnt extractable\n");
			printf("YEKP table isnt extractable, dumping subarc instead\n");
			snprintf( outputname, 64, "subarc-%02d.bin", i );
			fseek( f, filelist[i].offset, SEEK_SET );
			workbuf = malloc(filelist[i].size);
			fread( workbuf, filelist[i].size, 1, f );
			if( !(o = fopen( outputname, "wb" ))) {
				printf("Couldnt open file %s\n", outputname);
				return 1;
			}
			fwrite( workbuf, filelist[i].size, 1, o );
			fclose(o);
			free(workbuf);
			continue;
		}
		fseek(f, (filelist[i].offset+subarchead.size), SEEK_SET);
		snprintf(outputname, 64, "subarc%02d", i);
		createDirectory(outputname);
		numFiles = (sections[1].offset - subarchead.size) / 8;
		entries = malloc( numFiles * 8 );
		for( j = 0; j < numFiles; j++ ) fread( &entries[j], sizeof(subarcEntry), 1, f );
		for( j = 0; j < numFiles; j++ ) {
			if(!(entries[j].size)) {
				printf("Skipping empty file %04d in subarc %02d\n", j, i);
				continue;
			}
			fseek(f, (filelist[i].offset+sections[1].offset+entries[j].offset), SEEK_SET);
			if(entries[j].size & (1 << 31)) {
				workbuf = malloc(entries[j].size & ~(1<<31));
				fread(workbuf, (entries[j].size & ~(1<<31)), 1, f);
				resultbuf = unpackBuffer(workbuf, &resultsize, &compressedsize);
				free(workbuf);
				/* NITRO common filetypes */
				if( !strncmp( (char *)resultbuf, "BCA0", 4 )) snprintf( outputname, 32, "subarc%02d/%04d-%08x.nsbca", i, j, filelist[i].offset+sections[1].offset+entries[j].offset );
				else if( !strncmp( (char *)resultbuf, "BMD0", 4 )) snprintf( outputname, 32, "subarc%02d/%04d-%08x.nsbmd", i, j, filelist[i].offset+sections[1].offset+entries[j].offset );
				else if( !strncmp( (char *)resultbuf, "BTX0", 4 )) snprintf( outputname, 32, "subarc%02d/%04d-%08x.nsbtx", i, j, filelist[i].offset+sections[1].offset+entries[j].offset );
				else if( !strncmp( (char *)resultbuf, "BTA0", 4 )) snprintf( outputname, 32, "subarc%02d/%04d-%08x.nsbta", i, j, filelist[i].offset+sections[1].offset+entries[j].offset );
				else if( !strncmp( (char *)resultbuf, "RNAN", 4 )) snprintf( outputname, 32, "subarc%02d/%04d-%08x.nanr", i, j, filelist[i].offset+sections[1].offset+entries[j].offset );
				else if( !strncmp( (char *)resultbuf, "RECN", 4 )) snprintf( outputname, 32, "subarc%02d/%04d-%08x.ncer", i, j, filelist[i].offset+sections[1].offset+entries[j].offset );
				else if( !strncmp( (char *)resultbuf, "RGCN", 4 )) snprintf( outputname, 32, "subarc%02d/%04d-%08x.ncgr", i, j, filelist[i].offset+sections[1].offset+entries[j].offset );
				else if( !strncmp( (char *)resultbuf, "RLCN", 4 )) snprintf( outputname, 32, "subarc%02d/%04d-%08x.nclr", i, j, filelist[i].offset+sections[1].offset+entries[j].offset );
				else if( !strncmp( (char *)resultbuf, "RCMN", 4 )) snprintf( outputname, 32, "subarc%02d/%04d-%08x.nmcr", i, j, filelist[i].offset+sections[1].offset+entries[j].offset );
				else if( !strncmp( (char *)resultbuf, "RCSN", 4 )) snprintf( outputname, 32, "subarc%02d/%04d-%08x.nscr", i, j, filelist[i].offset+sections[1].offset+entries[j].offset );
				else if( !strncmp( (char *)resultbuf, "NARC", 4 )) snprintf( outputname, 32, "subarc%02d/%04d-%08x.narc", i, j, filelist[i].offset+sections[1].offset+entries[j].offset );
				else snprintf(outputname, 64, "subarc%02d/%04d-%08x-compressed.bin", i, j, (filelist[i].offset+sections[1].offset+entries[j].offset));
			}
			else {
				resultbuf = malloc(entries[j].size);
				fread(resultbuf, entries[j].size, 1, f);
				resultsize = entries[j].size;
				/* NITRO common filetypes */
				if( !strncmp( (char *)workbuf, "BCA0", 4 )) snprintf( outputname, 32, "subarc%02d/%04d-%08x.nsbca", i, j, filelist[i].offset+sections[1].offset+entries[j].offset );
				else if( !strncmp( (char *)workbuf, "BMD0", 4 )) snprintf( outputname, 32, "subarc%02d/%04d-%08x.nsbmd", i, j, filelist[i].offset+sections[1].offset+entries[j].offset );
				else if( !strncmp( (char *)workbuf, "BTX0", 4 )) snprintf( outputname, 32, "subarc%02d/%04d-%08x.nsbtx", i, j, filelist[i].offset+sections[1].offset+entries[j].offset );
				else if( !strncmp( (char *)workbuf, "BTA0", 4 )) snprintf( outputname, 32, "subarc%02d/%04d-%08x.nsbta", i, j, filelist[i].offset+sections[1].offset+entries[j].offset );
				else if( !strncmp( (char *)workbuf, "RNAN", 4 )) snprintf( outputname, 32, "subarc%02d/%04d-%08x.nanr", i, j, filelist[i].offset+sections[1].offset+entries[j].offset );
				else if( !strncmp( (char *)workbuf, "RECN", 4 )) snprintf( outputname, 32, "subarc%02d/%04d-%08x.ncer", i, j, filelist[i].offset+sections[1].offset+entries[j].offset );
				else if( !strncmp( (char *)workbuf, "RGCN", 4 )) snprintf( outputname, 32, "subarc%02d/%04d-%08x.ncgr", i, j, filelist[i].offset+sections[1].offset+entries[j].offset );
				else if( !strncmp( (char *)workbuf, "RLCN", 4 )) snprintf( outputname, 32, "subarc%02d/%04d-%08x.nclr", i, j, filelist[i].offset+sections[1].offset+entries[j].offset );
				else if( !strncmp( (char *)workbuf, "RCMN", 4 )) snprintf( outputname, 32, "subarc%02d/%04d-%08x.nmcr", i, j, filelist[i].offset+sections[1].offset+entries[j].offset );
				else if( !strncmp( (char *)workbuf, "RCSN", 4 )) snprintf( outputname, 32, "subarc%02d/%04d-%08x.nscr", i, j, filelist[i].offset+sections[1].offset+entries[j].offset );
				else if( !strncmp( (char *)workbuf, "NARC", 4 )) snprintf( outputname, 32, "subarc%02d/%04d-%08x.narc", i, j, filelist[i].offset+sections[1].offset+entries[j].offset );
				else snprintf(outputname, 64, "subarc%02d/%04d-%08x-uncompressed.bin", i, j, (filelist[i].offset+sections[1].offset+entries[j].offset));
			}
			if( !(o = fopen( outputname, "wb" ))) {
				printf("Couldnt open file %s\n", outputname);
				return 1;
			}
			fwrite(resultbuf, resultsize, 1, o);
			fclose(o);
			free(resultbuf);
			printf("File %04d - unpaksize %08x\n\n", j, resultsize);
		}
		free(entries);
	}
	
	fclose(f);
	printf("Done.\n");
	return 0;
}
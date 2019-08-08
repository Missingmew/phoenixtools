#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <zlib.h>

#ifdef _WIN32
#include <direct.h>
#define createDirectory(dirname) mkdir(dirname)
#else
#include <sys/stat.h>
#include <sys/types.h>
#define createDirectory(dirname) mkdir(dirname, 0777)
#endif

typedef struct {
	uint16_t mbname[32];
	uint32_t offset;
	uint32_t size;
} __attribute__((packed)) linkfile;

int writeFile( FILE *input, int length, FILE *output ) {
	unsigned char dataBuffer[1024];
	unsigned int bytesLeft = length;
	
	while(bytesLeft) {
		unsigned int wantedRead;
		if(bytesLeft >= sizeof(dataBuffer))
			wantedRead = sizeof(dataBuffer);
		else
			wantedRead = bytesLeft;
		unsigned int haveRead = fread(dataBuffer, 1, wantedRead, input);
		if(haveRead != wantedRead) {
			printf("haveRead != wantedRead: %d != %d\n", haveRead, wantedRead);
			perror("This broke");
			return 0;
		}

		unsigned int haveWrite = fwrite(dataBuffer, 1, haveRead, output);
		if(haveWrite != haveRead) {
			printf("haveWrite != haveRead: %d != %d\n", haveWrite, haveRead);
			return 0;
		}
		
		bytesLeft -= haveRead;
	}
	return 1;
}

void cleanDirname(char *name, unsigned int length) {
	unsigned int i;
	for(i = 0; i < length; i++) if(name[i] == '\\') name[i] = '/';
	return;
}

void makepath(char *path) {
	FILE *tmp = NULL;
	if((tmp = fopen(path, "wb"))) {
		remove(path);
		fclose(tmp);
		return;
	}
	char bakpath[2048] = {0}, curpath[2048] = {0};
	char *token = NULL;
	strcpy(bakpath, path);
	token = strtok(bakpath, "/");
	strcpy(curpath, token);
	
	while(token) {
		createDirectory(curpath);
		printf("creating directory %s\n", curpath);
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

int main( int argc, char **argv ) {
	
	char *name = NULL;
	unsigned char *comdata = NULL, *uncomdata = NULL;
	unsigned int i, j;
	
	uint32_t numfiles, workfiles, uncomsize, comsize;
	unsigned long int havebuf;
	
	linkfile *linkfiles = NULL;
	
	FILE *f, *o;
	
	if( argc < 3 ) {
		printf("Not enough args!\nUse: %s mode file\n", argv[0]);
		printf("supported modes: [p]lain, [d]ll, [l]inkdll\n");
		return 1;
	}
	
	if( !(f = fopen( argv[2], "rb" ))) {
		printf("Couldnt open file %s\n", argv[1]);
		return 1;
	}
	
	switch(*argv[1]) {
		case 'p': {
			name = malloc(0x100);
			fread(name, 0x100, 1, f);
			fread(&uncomsize, 4, 1, f);
			printf("Contained file: %s, size %08X bytes\n", name, uncomsize);
			
			if( !(o = fopen( name, "wb" ))) {
				printf("Couldnt open output %s\n", name);
				return 1;
			}
			
			writeFile(f, uncomsize, o);
			fclose(o);
			free(name);
			break;
		}
		
		case 'd': {
			fread(&uncomsize, 4, 1, f);
			fread(&comsize, 4, 1, f);
			uncomdata = malloc(uncomsize);
			comdata = malloc(comsize);
			fread(comdata, comsize, 1, f);
			havebuf = uncomsize;
			if((workfiles = uncompress(uncomdata, &havebuf, comdata, comsize)) != Z_OK) {
				printf("=====================================couldnt decompress data\n");
				switch(workfiles) {
					case Z_MEM_ERROR: {
						printf("memory error\n");
						break;
					}
					case Z_NEED_DICT: {
						printf("need dictionary\n");
						break;
					}
					case Z_DATA_ERROR: {
						printf("input corrupted\n");
						//~ printf("%s\n", strm.msg);
						break;
					}
					case Z_STREAM_ERROR: {
						printf("broken stream\n");
						break;
					}
					case Z_BUF_ERROR: {
						printf("buffer error\n");
						break;
					}
					default: {
						printf("unknown error\n");
						break;
					}
				}
				printf("uncomsize %08x\n", uncomsize);
				return 1;
			}
			
			name = malloc(strlen(argv[2]) + 1 + 4 + 1);
			sprintf(name, "%s.zdec", argv[2]);
			
			if( !(o = fopen( name, "wb" ))) {
				printf("Couldnt open output %s\n", name);
				return 1;
			}
			
			fwrite(uncomdata, uncomsize, 1, o);
			fclose(o);
			free(name);
			free(comdata);
			free(uncomdata);
			break;
		}
		
		case 'l': {
			fread(&workfiles, 4, 1, f);
			if(workfiles != 0x4B4E494C) {
				printf("not a linkdll\n");
				return 1;
			}
			
			fread(&numfiles, 4, 1, f);
			
			linkfiles = malloc(sizeof(linkfile) * numfiles);
			
			fread(linkfiles, sizeof(linkfile) * numfiles, 1, f);
			
			name = malloc(32);
			
			for(i = 0; i < numfiles; i++) {
				for(j = 0; j < 32; j++) name[j] = linkfiles[i].mbname[j] & 0xFF;
				
				printf("Current file %s, offset %08x, size %08x\n", name, linkfiles[i].offset, linkfiles[i].size);
				cleanDirname(name, 32);
				makepath(name);
				
				fseek(f, linkfiles[i].offset, SEEK_SET);
				
				if( !(o = fopen( name, "wb" ))) {
					printf("Couldnt open output %s\n", name);
					return 1;
				}
				
				writeFile(f, linkfiles[i].size, o);
				fclose(o);
			}
			free(name);
			free(linkfiles);
			break;
		}
		
		default: {
			printf("unknown mode %c\n", *argv[1]);
			break;
		}
	}
	
	fclose(f);
	printf("Done.\n");
	return 0;
}
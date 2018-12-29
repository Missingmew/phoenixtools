#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "ntrcom/nitrocompression.h"
#include "phoenixgfx.h"
#include "lodepng/lodepng.h"

#define BREAK(x) { printf(x); return 1; }
#define VOIDBREAK(x) { printf(x); return; }

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
}__attribute__((packed)) pacEntry;

typedef struct {
	unsigned char magic[4];
	uint32_t headerSize;
	uint32_t imageSize;
	uint32_t paletteOffset;
	uint32_t paletteSize;
}__attribute__((packed)) controlHeader;

char *typestring[] = {
	"rawdump", "rawgraphics", "compressedgraphics", "stripedgraphics", "patchgraphics", "uncompressedheadergraphics"
};

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

void extractTiledRawGraphic(FILE *f, unsigned int offset, char *path, unsigned int width, unsigned int height, unsigned int bpp, unsigned int paletteoffset) {
	unsigned int palettesize;
	unsigned int pixelsize;
	unsigned int filesize;
	unsigned char *workbuf = NULL, *palettebuf = NULL, *rgbapixeldata = NULL;
	pixelsize = width*8 * height*8;
	if(bpp == image4bpp) {
		palettesize = 16*2;
		pixelsize /= 2;
	}
	else if(bpp == image8bpp) {
		palettesize = 256*2;
	}
	filesize = pixelsize * palettesize;
	if(!paletteoffset) {
		workbuf = malloc(filesize);
		fseek(f, offset, SEEK_SET);
		fread(workbuf, filesize, 1, f);
		rgbapixeldata = tiledImageToRGBA(workbuf, width, height, bpp, 1);
	}
	else {
		workbuf = malloc(pixelsize);
		palettebuf = malloc(palettesize);
		fseek(f, offset, SEEK_SET);
		fread(workbuf, pixelsize, 1, f);
		fseek(f, paletteoffset, SEEK_SET);
		fread(palettebuf, palettesize, 1, f);
		rgbapixeldata = tiledImageWithPaletteToRGBA(workbuf, palettebuf, width, height, bpp, 1);
		free(palettebuf);
	}
	
	lodepng_encode32_file(path, rgbapixeldata, width*8, height*8);
	free(workbuf);
	free(rgbapixeldata);
	return;
}

void extractCompressedGraphic(FILE *f, unsigned int datasize, unsigned int offset, char *path, unsigned int width, unsigned int height, unsigned int bpp) {
	uint32_t tempsize = 0;
	unsigned char *workbuf = NULL, *resultbuf = NULL, *rgbapixeldata = NULL;
	unsigned int ressize, comsize;
	fseek(f, offset+1, SEEK_SET);
	fread(&tempsize, 3, 1, f);
	if((offset + 2*tempsize) > datasize) tempsize = datasize - offset;
	else tempsize *= 2;
	fseek(f, offset, SEEK_SET);
	workbuf = malloc(tempsize);
	fread(workbuf, tempsize, 1, f);
	resultbuf = unpackBuffer(workbuf, &ressize, &comsize);
	if(bpp == image8bpp) {
		if((width*8*height*8+512) != ressize) VOIDBREAK("size/bpp dont match with uncompressed size\n");
	}
	else if(bpp == image4bpp) {
		if(((width*8*height*8)/2+32) != ressize) VOIDBREAK("size/bpp dont match with uncompressed size\n");
	}
	if(!resultbuf) VOIDBREAK("couldnt decompress\n");
	rgbapixeldata = tiledImageToRGBA(resultbuf, width, height, bpp, 1);
	lodepng_encode32_file(path, rgbapixeldata, width*8, height*8);
	free(workbuf);
	free(resultbuf);
	free(rgbapixeldata);
	return;
}

void extractStripedGraphic(FILE *f, unsigned int offset, char *path, unsigned int width, unsigned int height) {
	pacEntry *entrylist;
	uint32_t numfiles;
	unsigned char *workbuf = NULL, *resultbuf = NULL, *rgbapixeldata = NULL, *palettedata = NULL, *fullbuffer = NULL, *fulltarget = NULL;
	unsigned int ressize, comsize, bpp, i;
	fseek(f, offset, SEEK_SET);
	fread(&numfiles, 4, 1, f);
	entrylist = malloc(sizeof(pacEntry)*numfiles);
	fread(entrylist, sizeof(pacEntry), numfiles, f);
	palettedata = malloc(entrylist[0].size);
	bpp = (entrylist[0].size == 32)?image4bpp:image8bpp;
	fseek(f, offset+entrylist[0].offset, SEEK_SET);
	fread(palettedata, entrylist[0].size, 1, f);
	
	fullbuffer = malloc(width*8*((numfiles-1)*height)*8);
	fulltarget = fullbuffer;
	
	for(i=1;i < numfiles;i++) {
		fseek(f, offset+entrylist[i].offset, SEEK_SET);
		workbuf = malloc(entrylist[i].size);
		fread(workbuf, entrylist[i].size, 1, f);
		resultbuf = unpackBuffer(workbuf, &ressize, &comsize);
		if(bpp == image8bpp) {
			if((width*8*height*8) != ressize) VOIDBREAK("size/bpp dont match with uncompressed size\n");
		}
		else if(bpp == image4bpp) {
			if(((width*8*height*8)/2) != ressize) VOIDBREAK("size/bpp dont match with uncompressed size\n");
		}
		if(!resultbuf) VOIDBREAK("couldnt decompress\n");
		memcpy(fulltarget, resultbuf, ressize);
		fulltarget += ressize;
		free(workbuf);
		free(resultbuf);
	}
	rgbapixeldata = tiledImageWithPaletteToRGBA(fullbuffer, palettedata, width, ((numfiles-1)*height), bpp, 1);
	lodepng_encode32_file(path, rgbapixeldata, width*8, ((numfiles-1)*height)*8);
	free(fullbuffer);
	free(rgbapixeldata);
	free(palettedata);
	free(entrylist);
	return;
}

void extractPatchGraphic(FILE *f, unsigned int datasize, unsigned int offset, char *path, unsigned int width, unsigned int height, unsigned int sourceoffset, unsigned int sourcetype) {
	pacEntry entry;
	uint32_t tempsize = 0;
	unsigned char *workbuf = NULL, *resultbuf = NULL, *rgbapixeldata = NULL, *palettedata = NULL;
	unsigned int ressize, comsize, bpp;
	if(!sourcetype) {
		fseek(f, sourceoffset+4, SEEK_SET);
		fread(&entry, sizeof(entry), 1, f);
		palettedata = malloc(entry.size);
		bpp = (entry.size == 32)?image4bpp:image8bpp;
		fseek(f, sourceoffset+entry.offset, SEEK_SET);
		fread(palettedata, entry.size, 1, f);
	}
	else {
		bpp = (sourcetype==8)?image8bpp:image4bpp;
		fseek(f, sourceoffset+1, SEEK_SET);
		fread(&tempsize, 3, 1, f);
		if((sourceoffset + 2*tempsize) > datasize) tempsize = datasize - sourceoffset;
		else tempsize *= 2;
		fseek(f, sourceoffset, SEEK_SET);
		workbuf = malloc(tempsize);
		fread(workbuf, tempsize, 1, f);
		resultbuf = unpackBuffer(workbuf, &ressize, &comsize);
		if(!resultbuf) {
			printf("couldnt decompress\n");
			return;
		}
		tempsize = (bpp == image4bpp)?32:512;
		palettedata = malloc(tempsize);
		memcpy(palettedata, resultbuf, tempsize);
		free(workbuf);
		free(resultbuf);
	}
	fseek(f, offset+1, SEEK_SET);
	fread(&tempsize, 3, 1, f);
	if((offset + 2*tempsize) > datasize) tempsize = datasize - offset;
	else tempsize *= 2;
	fseek(f, offset, SEEK_SET);
	workbuf = malloc(tempsize);
	fread(workbuf, tempsize, 1, f);
	resultbuf = unpackBuffer(workbuf, &ressize, &comsize);
	if(bpp == image8bpp) {
		if((width*8*height*8) != ressize) VOIDBREAK("size/bpp8 dont match with uncompressed size\n");
	}
	else if(bpp == image4bpp) {
		if(((width*8*height*8)/2) != ressize) VOIDBREAK("size/bpp4 dont match with uncompressed size\n");
	}
	if(!resultbuf) VOIDBREAK("couldnt decompress\n");
	rgbapixeldata = tiledImageWithPaletteToRGBA(resultbuf, palettedata, width, height, bpp, 0);
	lodepng_encode32_file(path, rgbapixeldata, width*8, height*8);
	free(rgbapixeldata);
	free(workbuf);
	free(resultbuf);
	free(palettedata);
	return;
}

void extractUncompressedHeaderGraphic(FILE *f, unsigned int offset, char *path) {
	controlHeader header;
	unsigned char *workbuf = NULL, *palettedata = NULL, *rgbapixeldata = NULL;
	unsigned int pixelsX, pixelsY, bpp;
	fseek(f, offset, SEEK_SET);
	fread(&header, sizeof(header), 1, f);
	
	/* lots of copypasta, see convert-uncompressed-image-header.c */
	if( header.magic[1] > 5 ) {
		printf("evil width %x in file\n", header.magic[1]);
		return;
	}
	else { 
		pixelsX = 8 << header.magic[1];
	}
	if( header.magic[2] > 5 ) {
		printf("evil height %x in file\n", header.magic[2]);
		return;
	}
	else {
		pixelsY = 8 << header.magic[2];
	}
	
	switch( header.magic[0] ) {
		/* 4bpp */
		case 3: {
			printf("image is 4bpp\n");
			bpp = image4bpp;
			if((header.imageSize) != ((pixelsX*pixelsY)/2)) VOIDBREAK("size doesnt match!\n");
			break;
		}
		/* 8bpp */
		case 4: {
			printf("image is 8bpp\n");
			bpp = image8bpp;
			if((header.imageSize) != ((pixelsX*pixelsY))) VOIDBREAK("size doesnt match!\n");
			break;
		}
		/* a5i3 */
		case 6: {
			printf("image is a5i3\n");
			bpp = imagea5i3;
			if((header.imageSize) != ((pixelsX*pixelsY))) VOIDBREAK("size doesnt match!\n");
			break;
		}
		/* fallback */
		default: {
			printf("fallback. textype is %08x\n", header.magic[0]);
			if( header.paletteSize < 0x200 ) {
				printf("image is 4bpp\n");
				bpp = image4bpp;
				if((header.imageSize) != ((pixelsX*pixelsY)/2)) VOIDBREAK("size doesnt match!\n");
			}
			else {
				printf("image is 8bpp\n");
				bpp = image8bpp;
				if((header.imageSize) != ((pixelsX*pixelsY))) VOIDBREAK("size doesnt match!\n");
			}
			break;
		}
	}
	if( bpp == imagea5i3 ) {
		if( header.paletteSize == 0x200 ) palettedata = malloc(header.paletteSize);
		else {
			palettedata = malloc(8*2);
			memset(palettedata, 0, 8*2);
		}
	}
	else if ( bpp == imagea3i5 ) {
		palettedata = malloc(32*2);
		memset(palettedata, 0, 32*2);
	}
	else palettedata = malloc(header.paletteSize);
	fseek(f, offset+header.paletteOffset, SEEK_SET);
	fread(palettedata, header.paletteSize, 1, f);
	fseek(f, offset+header.headerSize, SEEK_SET);
	workbuf = malloc(header.imageSize);
	fread(workbuf, header.imageSize, 1, f);
	
	rgbapixeldata = linearImageWithPaletteToRGBA(workbuf, palettedata, pixelsX, pixelsY, bpp, 1);
	lodepng_encode32_file(path, rgbapixeldata, pixelsX, pixelsY);
	free(workbuf);
	free(rgbapixeldata);
	free(palettedata);
	return;
}
	

int main(int argc, char** argv) {
	
	if ( argc < 3 ) {
		printf("Not enough arguments given!\nUsage: %s file textmap\n", argv[0]);
		return 1;
	}
	
	FILE *data = NULL, *map = NULL, *o = NULL;
	char mapline[2048] = {0};
	unsigned char *tempbuf = NULL;
	/* arguments used by all types */
	char outputpath[2048] = {0};
	unsigned int i = 0, params = 0, datasize = 0, fileoffset = 0, filetype = 0;
	
	/* arguments used by specific types */
	unsigned int filesize;
	unsigned int tilesx, tilesy, sourcetype, sourceoffset, bpp;
	
	if( !(data = fopen(argv[1], "rb"))) {
		printf("Couldnt open %s for reading\n", argv[1]);
		return 1;
	}
	fseek(data, 0, SEEK_END);
	datasize = ftell(data);
	fseek(data, 0, SEEK_SET);
	if( !(map = fopen(argv[2], "r"))) {
		printf("Couldnt open %s for reading\n", argv[2]);
		return 1;
	}
	
	for(;;) {
		i++;
		fgets(mapline, 2048, map);
		if(feof(map)) break;
		if(!(strncmp(mapline, "//", 2))) {
			printf("line %03d - commented out\n", i);
			continue;
		}
		if(strlen(mapline) < 3) {
			printf("line %03d - empty\n", i);
			continue;
		}
		params = sscanf(mapline, "%x %d \"%[^\"]\"", &fileoffset, &filetype, outputpath);
		if(!params) continue;
		if(params < 3) {
			printf("Malformed params at line %d, wanted 3, got %d\n", i, params);
			continue;
		}
		if(strlen(outputpath)<3) {
			printf("invalid path at line %d (%u)\n", i, (unsigned int)strlen(outputpath));
			continue;
		}
		printf("line %03d - Offset %08x - Type %s(%d) - %s\n", i, fileoffset, typestring[filetype], filetype, outputpath);
		makepath(outputpath);
		switch(filetype) {
			case 0: { /* rawdump */
				params = sscanf(mapline, "%x %d \"%[^\"]\" %x", &fileoffset, &filetype, outputpath, &filesize);
				if(params < 4) {
					printf("Not enough params for rawdump at line %d, wanted 4, got %d\n", i, params);
					continue;
				}
				if(!(o = fopen(outputpath, "wb"))) {
					printf("Couldnt open %s for writing\n", outputpath);
					break;
				}
				fseek(data, fileoffset, SEEK_SET);
				tempbuf = malloc(filesize);
				fread(tempbuf, filesize, 1, data);
				fwrite(tempbuf, filesize, 1, o);
				free(tempbuf);
				fclose(o);
				break;
			}
			case 1: { /* tiled rawgraphics */
				printf("Raw graphics arent supported right now\n");
				/* if paletteoffset is 0, assume palette in front of image data */
				/* params = sscanf(mapline, "%x %d \"%[^\"]\" %d %d %x", &fileoffset, &filetype, outputpath, &tilesx, &tilesy, &sourcetype, &paletteoffset);
				bpp = (sourcetype == 8)?image8bpp:image4bpp;
				extractTiledRawRaphic(data, fileoffset, outputpath, tilesx, tilesy, bpp, paletteoffset);*/
				break;
			}
			case 2: { /* compressed graphics */
				params = sscanf(mapline, "%x %d \"%[^\"]\" %d %d %d", &fileoffset, &filetype, outputpath, &tilesx, &tilesy, &sourcetype);
				if(params < 6) {
					printf("Not enough params for comgfx at line %d, wanted 6, got %d\n", i, params);
					continue;
				}
				bpp = (sourcetype == 8)?image8bpp:image4bpp;
				extractCompressedGraphic(data, datasize, fileoffset, outputpath, tilesx, tilesy, bpp);
				break;
			}
			case 3: { /* striped graphics */
				params = sscanf(mapline, "%x %d \"%[^\"]\" %d %d", &fileoffset, &filetype, outputpath, &tilesx, &tilesy);
				if(params < 5) {
					printf("Not enough params for sgfx at line %d, wanted 5, got %d\n", i, params);
					continue;
				}
				extractStripedGraphic(data, fileoffset, outputpath, tilesx, tilesy);
				break;
			}
			case 4: { /* patch graphics */
				params = sscanf(mapline, "%x %d \"%[^\"]\" %d %d %x %d", &fileoffset, &filetype, outputpath, &tilesx, &tilesy, &sourceoffset, &sourcetype);
				if(params < 7) {
					printf("Not enough params for pgfx at line %d, wanted 7, got %d\n", i, params);
					continue;
				}
				extractPatchGraphic(data, datasize, fileoffset, outputpath, tilesx, tilesy, sourceoffset, sourcetype);
				break;
			}
			case 5: { /*uncompressed header graphics */
				/* dont need to sscanf again, since we dont need additional data if everything is okay */
				extractUncompressedHeaderGraphic(data, fileoffset, outputpath);
				break;
			}
		}
	}
	fclose(data);
	fclose(map);
	printf("Done.\n");
	return 0;
}
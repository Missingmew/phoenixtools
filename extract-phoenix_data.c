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

typedef struct {
	unsigned int datasize;
	unsigned int fileoffset;
	unsigned int filesize;
	char *palpath;
	char *gfxpath;
	unsigned int width;
	unsigned int height;
	unsigned int bpp;
	unsigned int sourcetype;
	unsigned int sourceoffset;
} outputinfo;

char *typestring[] = {
	"rawdump", "compressedgraphics", "stripedgraphics", "patchgraphics", "uncompressedheadergraphics"
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

void dumpRaw(FILE *f, outputinfo *info) {
	FILE *o;
	unsigned char *tempbuf = NULL;
	
	makepath(info->gfxpath);
	
	if(!(o = fopen(info->gfxpath, "wb"))) {
		printf("dumpRaw: Couldnt open %s for writing\n", info->gfxpath);
		return;
	}
	fseek(f, info->fileoffset, SEEK_SET);
	tempbuf = malloc(info->filesize);
	fread(tempbuf, info->filesize, 1, f);
	fwrite(tempbuf, info->filesize, 1, o);
	free(tempbuf);
	fclose(o);
}

void dumpCompressed(FILE *f, outputinfo *info) {
	FILE *o;
	uint32_t tempsize = 0;
	unsigned char *workbuf = NULL, *resultbuf = NULL;
	unsigned int ressize, comsize;
	
	makepath(info->palpath);
	makepath(info->gfxpath);
	
	// there is probably a better way to calculate the compressed size, even if we assume >100% compression ratio
	fseek(f, info->fileoffset+1, SEEK_SET);
	fread(&tempsize, 3, 1, f);
	if((info->fileoffset + 2*tempsize) > info->datasize) tempsize = info->datasize - info->fileoffset;
	else tempsize *= 2;
	comsize = tempsize;
	workbuf = malloc(tempsize);
	
	fseek(f, info->fileoffset, SEEK_SET);
	fread(workbuf, tempsize, 1, f);
	resultbuf = unpackBuffer(workbuf, &ressize, &comsize);
	if(info->bpp == image8bpp) {
		if((info->width*8*info->height*8+512) != ressize) VOIDBREAK("size/bpp dont match with uncompressed size\n");
	}
	else if(info->bpp == image4bpp) {
		if(((info->width*8*info->height*8)/2+32) != ressize) VOIDBREAK("size/bpp dont match with uncompressed size\n");
	}
	if(!resultbuf) VOIDBREAK("couldnt decompress\n");
	
	if(!(o = fopen(info->palpath, "wb"))) {
		printf("dumpRaw: Couldnt open %s for writing\n", info->palpath);
		return;
	}
	fwrite(resultbuf, (info->bpp == image8bpp) ? 512 : 32, 1, o);
	fclose(o);
	
	if(!(o = fopen(info->gfxpath, "wb"))) {
		printf("dumpRaw: Couldnt open %s for writing\n", info->gfxpath);
		return;
	}
	if(info->bpp == image8bpp) fwrite(resultbuf+512, (info->width*8*info->height*8), 1, o);
	else  fwrite(resultbuf+32, (info->width*8*info->height*8)/2, 1, o);
	fclose(o);
	
	free(workbuf);
	free(resultbuf);
	return;
}

void dumpStriped(FILE *f, outputinfo *info) {
	FILE *o;
	pacEntry *entrylist;
	uint32_t numfiles;
	unsigned char *workbuf = NULL, *resultbuf = NULL, *palettedata = NULL, *fullbuffer = NULL, *fulltarget = NULL;
	unsigned int ressize, comsize, bpp, i, fullsize;
	
	makepath(info->palpath);
	makepath(info->gfxpath);
	
	fseek(f, info->fileoffset, SEEK_SET);
	fread(&numfiles, 4, 1, f);
	entrylist = malloc(sizeof(pacEntry)*numfiles);
	fread(entrylist, sizeof(pacEntry), numfiles, f);
	
	palettedata = malloc(entrylist[0].size);
	bpp = (entrylist[0].size == 32)?image4bpp:image8bpp;
	fseek(f, info->fileoffset+entrylist[0].offset, SEEK_SET);
	fread(palettedata, entrylist[0].size, 1, f);
	
	fullsize = info->width*8*((numfiles-1)*info->height)*8;
	if(bpp == image4bpp) fullsize /= 2;
	fullbuffer = malloc(fullsize);
	fulltarget = fullbuffer;
	
	for(i=1;i < numfiles;i++) {
		fseek(f, info->fileoffset+entrylist[i].offset, SEEK_SET);
		workbuf = malloc(entrylist[i].size);
		fread(workbuf, entrylist[i].size, 1, f);
		comsize = entrylist[i].size;
		resultbuf = unpackBuffer(workbuf, &ressize, &comsize);
		if(bpp == image8bpp) {
			if((info->width*8*info->height*8) != ressize) VOIDBREAK("size/bpp dont match with uncompressed size\n");
		}
		else if(bpp == image4bpp) {
			if(((info->width*8*info->height*8)/2) != ressize) VOIDBREAK("size/bpp dont match with uncompressed size\n");
		}
		if(!resultbuf) VOIDBREAK("couldnt decompress\n");
		memcpy(fulltarget, resultbuf, ressize);
		fulltarget += ressize;
		free(workbuf);
		free(resultbuf);
	}
	
	if(!(o = fopen(info->palpath, "wb"))) {
		printf("dumpRaw: Couldnt open %s for writing\n", info->palpath);
		return;
	}
	fwrite(palettedata, entrylist[0].size, 1, o);
	fclose(o);
	
	strcat(info->gfxpath, (bpp == image8bpp) ? ".8bpp" : ".4bpp");
	if(!(o = fopen(info->gfxpath, "wb"))) {
		printf("dumpRaw: Couldnt open %s for writing\n", info->gfxpath);
		return;
	}
	fwrite(fullbuffer, fullsize, 1, o);
	fclose(o);
	
	free(fullbuffer);
	free(palettedata);
	free(entrylist);
	return;
}

void dumpPatch(FILE *f, outputinfo *info) {
	pacEntry entry;
	uint32_t tempsize = 0;
	unsigned char *workbuf = NULL, *resultbuf = NULL;
	unsigned int ressize, comsize, bpp;
	FILE *o;
	
	makepath(info->gfxpath);
	
	if(!info->sourcetype) {
		fseek(f, info->sourceoffset+4, SEEK_SET);
		fread(&entry, sizeof(entry), 1, f);
		bpp = (entry.size == 32)?image4bpp:image8bpp;
	}
	else {
		bpp = (info->sourcetype==8)?image8bpp:image4bpp;
	}
	fseek(f, info->fileoffset+1, SEEK_SET);
	fread(&tempsize, 3, 1, f);
	if((info->fileoffset + 2*tempsize) > info->datasize) tempsize = info->datasize - info->fileoffset;
	else tempsize *= 2;
	comsize = tempsize;
	
	fseek(f, info->fileoffset, SEEK_SET);
	workbuf = malloc(tempsize);
	fread(workbuf, tempsize, 1, f);
	resultbuf = unpackBuffer(workbuf, &ressize, &comsize);
	if(bpp == image8bpp) {
		if((info->width*8*info->height*8) != ressize) VOIDBREAK("size/bpp8 dont match with uncompressed size\n");
	}
	else if(bpp == image4bpp) {
		if(((info->width*8*info->height*8)/2) != ressize) VOIDBREAK("size/bpp4 dont match with uncompressed size\n");
	}
	if(!resultbuf) VOIDBREAK("couldnt decompress\n");
	
	if(!(o = fopen(info->gfxpath, "wb"))) {
		printf("dumpRaw: Couldnt open %s for writing\n", info->gfxpath);
		return;
	}
	fwrite(resultbuf, ressize, 1, o);
	fclose(o);
	
	free(workbuf);
	free(resultbuf);
	return;
}

void dumpUncompressed(FILE *f, outputinfo *info) {
	controlHeader header;
	unsigned char *workbuf = NULL, *palettedata = NULL, *rgbapixeldata = NULL;
	unsigned int pixelsX, pixelsY, bpp;
	
	makepath(info->gfxpath);
	
	fseek(f, info->fileoffset, SEEK_SET);
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
			//~ printf("image is 4bpp\n");
			bpp = image4bpp;
			if((header.imageSize) != ((pixelsX*pixelsY)/2)) VOIDBREAK("size doesnt match!\n");
			break;
		}
		/* 8bpp */
		case 4: {
			//~ printf("image is 8bpp\n");
			bpp = image8bpp;
			if((header.imageSize) != ((pixelsX*pixelsY))) VOIDBREAK("size doesnt match!\n");
			break;
		}
		/* a5i3 */
		case 6: {
			//~ printf("image is a5i3\n");
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
	fseek(f, info->fileoffset+header.paletteOffset, SEEK_SET);
	fread(palettedata, header.paletteSize, 1, f);
	fseek(f, info->fileoffset+header.headerSize, SEEK_SET);
	workbuf = malloc(header.imageSize);
	fread(workbuf, header.imageSize, 1, f);
	
	rgbapixeldata = linearImageWithPaletteToRGBA(workbuf, palettedata, pixelsX, pixelsY, bpp, 1);
	lodepng_encode32_file(info->gfxpath, rgbapixeldata, pixelsX, pixelsY);
	free(workbuf);
	free(rgbapixeldata);
	free(palettedata);
	return;
}

void (*dumpfuncs[])(FILE *, outputinfo *) = {
	dumpRaw,
	dumpCompressed,
	dumpStriped,
	dumpPatch,
	dumpUncompressed
};

int main(int argc, char** argv) {
	FILE *data = NULL, *map = NULL;
	char mapline[2048] = {0};
	/* arguments used by all types */
	char outputpath[2048] = {0}, palettepath[2048] = {0}, graphicspath[2048] = {0};
	unsigned int i = 0, params = 0, datasize = 0, fileoffset = 0, filetype = 0;
	
	/* arguments used by specific types */
	unsigned int filesize;
	unsigned int tilesx, tilesy, sourcetype, sourceoffset, bpp;
	
	outputinfo outinfo;
	
	if ( argc < 3 ) {
		printf("Not enough arguments given!\nUsage: %s file textmap\n", argv[0]);
		return 1;
	}
	
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
		printf("line %03d - Offset %08x - Type %s(%d) - %s\n", i, fileoffset, typestring[filetype], filetype, outputpath);
		switch(filetype) {
			case 0: { /* rawdump */
				params = sscanf(mapline, "%x %d \"%[^\"]\" %x", &fileoffset, &filetype, graphicspath, &filesize);
				if(params < 4) {
					printf("Not enough params for rawdump at line %d, wanted 4, got %d\n", i, params);
					continue;
				}
				if(strlen(graphicspath)<3) {
					printf("invalid output path at line %d (%u)\n", i, (unsigned int)strlen(graphicspath));
					continue;
				}
				break;
			}
			case 1: { /* compressed graphics */
				params = sscanf(mapline, "%x %d \"%[^\"]\" \"%[^\"]\" %d %d %d", &fileoffset, &filetype, palettepath, graphicspath, &tilesx, &tilesy, &sourcetype);
				if(params < 7) {
					printf("Not enough params for comgfx at line %d, wanted 7, got %d\n", i, params);
					continue;
				}
				if(strlen(palettepath)<3) {
					printf("invalid palette path at line %d (%u)\n", i, (unsigned int)strlen(palettepath));
					continue;
				}
				if(strlen(graphicspath)<3) {
					printf("invalid graphics path at line %d (%u)\n", i, (unsigned int)strlen(graphicspath));
					continue;
				}
				bpp = (sourcetype == 8)?image8bpp:image4bpp;
				break;
			}
			case 2: { /* striped graphics */
				params = sscanf(mapline, "%x %d \"%[^\"]\" \"%[^\"]\" %d %d", &fileoffset, &filetype, palettepath, graphicspath, &tilesx, &tilesy);
				if(params < 6) {
					printf("Not enough params for sgfx at line %d, wanted 5, got %d\n", i, params);
					continue;
				}
				if(strlen(palettepath)<3) {
					printf("invalid palette path at line %d (%u)\n", i, (unsigned int)strlen(palettepath));
					continue;
				}
				if(strlen(graphicspath)<3) {
					printf("invalid graphics path at line %d (%u)\n", i, (unsigned int)strlen(graphicspath));
					continue;
				}
				break;
			}
			case 3: { /* patch graphics */
				params = sscanf(mapline, "%x %d \"%[^\"]\" %d %d %x %d", &fileoffset, &filetype, graphicspath, &tilesx, &tilesy, &sourceoffset, &sourcetype);
				if(params < 7) {
					printf("Not enough params for pgfx at line %d, wanted 7, got %d\n", i, params);
					continue;
				}
				if(strlen(graphicspath)<3) {
					printf("invalid output path at line %d (%u)\n", i, (unsigned int)strlen(graphicspath));
					continue;
				}
				break;
			}
			case 4: { /*uncompressed header graphics */
				/* dont need to sscanf again, since we dont need additional data if everything is okay */
				break;
			}
			default: {
				printf("unknown type %d in line %d, skipping...\n", filetype, i);
				continue;
			}
		}
		outinfo.datasize = datasize;
		outinfo.fileoffset = fileoffset;
		outinfo.filesize = filesize;
		outinfo.palpath = palettepath;
		outinfo.gfxpath = graphicspath;
		outinfo.width = tilesx;
		outinfo.height = tilesy;
		outinfo.bpp = bpp;
		outinfo.sourcetype = sourcetype;
		outinfo.sourceoffset = sourceoffset;
		
		dumpfuncs[filetype](data, &outinfo);
	}
	fclose(data);
	fclose(map);
	printf("Done.\n");
	return 0;
}
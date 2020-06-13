#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "lodepng/lodepng.h"
#include "phoenixgfx.h"

struct objsize {
	unsigned w, h;
};

/* access via sizelut[shape][size] */
struct objsize sizelut[3][4] = {
	/* square */
	{
		{ .w = 8, .h = 8 },
		{ .w = 16, .h = 16 },
		{ .w = 32, .h = 32 },
		{ .w = 64, .h = 64 },
	},
	/* horizontal */
	{
		{ .w = 16, .h = 8 },
		{ .w = 32, .h = 8 },
		{ .w = 32, .h = 16 },
		{ .w = 64, .h = 32 },
	},
	/* vertical */
	{
		{ .w = 8, .h = 16 },
		{ .w = 8, .h = 32 },
		{ .w = 16, .h = 32 },
		{ .w = 32, .h = 64 },
	},
	/* not allowed */
};

struct animetatile {
	signed x, y;
	unsigned w, h;
	unsigned index;
};

struct partdesc {
	signed x:8;
	signed y:8;
	unsigned tilenum:10;
	unsigned pad:2;
	unsigned shape:2;
	unsigned size:2;
}__attribute__((packed));

struct animationhead {
	uint16_t null;
	uint16_t numentries;
	uint32_t pad;
}__attribute__((packed));

struct animationframe {
	uint16_t offset;
	uint8_t duration;
	uint8_t palmod;
	uint32_t null;
}__attribute__((packed));


unsigned char *deRLE(uint16_t *src, unsigned decompressedsize) {
	uint16_t *ret = malloc(decompressedsize);
	unsigned destidx = 0, srcidx = 0, i;
	unsigned isrepeat, len;
	while(destidx < decompressedsize/2) {
		isrepeat = src[srcidx] & 0x8000;
		len = src[srcidx] & 0x7FFF;
		srcidx++;
		if(isrepeat) {
			for(i = 0; i < len; i++) ret[destidx++] = src[srcidx];
			srcidx++;
		}
		else {
			for(i = 0; i < len; i++) ret[destidx++] = src[srcidx++];
		}
	}
	//~ printf("expanded %x bytes to %x\n", srcidx*2, destidx*2);
	return (unsigned char *)ret;
}

/* stores tiles from src at dest according to meta, assumes all dimensions are in pixels */
void blit_metatile(unsigned char *dest, uint16_t **src, struct animetatile *meta, unsigned imagewidth, unsigned bpp) {
	unsigned tilesize;
	unsigned h;
	unsigned char *blitdest = NULL, *srctiles = NULL, *srclinear = NULL;
	
	unsigned metawtiles, metahtiles;
	metawtiles = meta->w/8;
	metahtiles = meta->h/8;
	
	if(bpp == image4bpp) tilesize = 32;
	else if(bpp == image8bpp) tilesize = 64;
	else {
		printf("%s: unknown bpp %u\n", __func__, bpp);
		return;
	}
	
	srctiles = deRLE(src[meta->index], metawtiles*metahtiles*tilesize);
	srclinear = generateIndexedImageFromTiles(srctiles, metawtiles, metahtiles, bpp, NULL);
	
	blitdest = dest + meta->x + imagewidth*meta->y;
	//~ printf("dest %p src %p mets %p imw %u bpp %u\n", dest, src, meta, imagewidth, bpp);
	//~ printf("skipsize %u tilesize %u blitdest %p (offset by %u)\n", skipsize, tilesize, blitdest, metaxtiles*tilesize + skipsize*metaytiles);
	//~ printf("meta: x=%u(%u) y=%u(%u) w=%u(%u) h=%u(%u) idx %u\n", meta->x, metaxtiles, meta->y, metaytiles, meta->w, metawtiles, meta->h, metahtiles, meta->index);
	for(h = 0; h < meta->h; h++) {
		memcpy(blitdest+h*imagewidth, srclinear+h*meta->w, meta->w);
	}
	
	free(srctiles);
	free(srclinear);
}

#define NDS_WIDTH 256
#define NDS_HEIGHT 192
#define NDS_IMAGESIZE (NDS_WIDTH*NDS_HEIGHT)
#define NDS_CENTER_X (NDS_WIDTH/2)
#define NDS_CENTER_Y (NDS_HEIGHT/2)

#define GBA_WIDTH 240
#define GBA_HEIGHT 160
#define GBA_IMAGESIZE (GBA_WIDTH*GBA_HEIGHT)
#define GBA_CENTER_X (GBA_WIDTH/2)
#define GBA_CENTER_Y (GBA_HEIGHT/2)

int main(int argc, char **argv) {
	FILE *gfx = NULL, *anim = NULL;
	char finaloutputname[256];
	
	unsigned gfxsize, animsize;
	void *gfxbuf = NULL, *animbuf = NULL;
	
	unsigned numpals;
	unsigned char **paldata = NULL;
	uint16_t **tiledata = NULL;
	void *gfxbase = NULL;
	uint32_t *partslist = NULL;
	
	struct animationhead *animhead = NULL;
	struct animationframe *animframes = NULL;
	uint32_t *numparts;
	struct partdesc *partheads = NULL;
	
	unsigned char *finalimage = NULL;
	unsigned char *finalrgba = NULL;
	
	if(argc < 4) {
		printf("not enough args! use %s graphics animation outputprefix\n", argv[0]);
		return 1;
	}
	
	if(!(gfx = fopen(argv[1], "rb"))) {
		printf("couldnt open %s for reading\n", argv[1]);
		return 1;
	}
	fseek(gfx, 0, SEEK_END);
	gfxsize = ftell(gfx);
	fseek(gfx, 0, SEEK_SET);
	gfxbuf = malloc(gfxsize);
	fread(gfxbuf, gfxsize, 1, gfx);
	fclose(gfx);
	/* NDS versions have MSB set for some reason, GBA ones dont */
	numpals = *((uint32_t *)gfxbuf) & 0x7FFFFFFF;
	paldata = malloc(sizeof(unsigned char *) * numpals);
	for(unsigned i = 0; i < numpals; i++) paldata[i] = gfxbuf+4+i*0x20;
	partslist = gfxbase = gfxbuf+4+numpals*0x20;
	tiledata = malloc((partslist[0] / 4)*sizeof(uint16_t *));
	for(unsigned i = 0; i < (partslist[0] / 4); i++) tiledata[i] = gfxbase+partslist[i];
	
	if(!(anim = fopen(argv[2], "rb"))) {
		printf("couldnt open %s for reading\n", argv[2]);
		return 1;
	}
	fseek(anim, 0, SEEK_END);
	animsize = ftell(anim);
	fseek(anim, 0, SEEK_SET);
	animbuf = malloc(animsize);
	fread(animbuf, animsize, 1, anim);
	fclose(anim);
	
	finalimage = malloc(NDS_IMAGESIZE);
	
	printf("table in gfx file has %u entries\n", partslist[0] / 4);
	
	/* only try first table for now */
	unsigned currenttable, endoftable, lastoffset, lastparts;
	for(currenttable = 0, endoftable = 0; endoftable < animsize; endoftable += lastoffset + lastparts * sizeof(uint32_t) + sizeof(uint32_t), currenttable++) {
		lastoffset = 0;
		lastparts = 0;
		printf("Table %u start at %08x\n", currenttable, endoftable);
		animhead = animbuf+endoftable;
		animframes = (void *)animhead+sizeof(struct animationhead);
		
		for(unsigned curframe = 0; curframe < animhead->numentries; curframe++) {
			memset(finalimage, 0, NDS_IMAGESIZE);
			printf("Frame %u dataoff %08x:\n", curframe, animframes[curframe].offset);
			numparts = (void *)animhead + animframes[curframe].offset;
			partheads = (void *)numparts + sizeof(uint32_t);
			for(unsigned curpart = 0; curpart < *numparts; curpart++) {
				struct animetatile metatile;
				//~ printf("part %2x x=%4d y=%4d shape=%u size=%u pad=%u w=%2u h=%2u tilenum=%4u\n", curpart, partheads[curpart].x, partheads[curpart].y, partheads[curpart].shape, partheads[curpart].size, partheads[curpart].pad, sizelut[partheads[curpart].shape][partheads[curpart].size].w, sizelut[partheads[curpart].shape][partheads[curpart].size].h, partheads[curpart].tilenum);
				metatile.x = partheads[curpart].x;
				metatile.y = partheads[curpart].y;
				metatile.w = sizelut[partheads[curpart].shape][partheads[curpart].size].w;
				metatile.h = sizelut[partheads[curpart].shape][partheads[curpart].size].h;
				metatile.index = partheads[curpart].tilenum;
				/* convert relative positions to absolute ones */
				metatile.x += NDS_CENTER_X;
				metatile.y += NDS_CENTER_Y;
				blit_metatile(finalimage, tiledata, &metatile, NDS_WIDTH, image4bpp);
			}
			
			sprintf(finaloutputname, "%s-tb%02u-fr%02u.png", argv[3], currenttable, curframe);
			finalrgba = linearImageWithPaletteToRGBA(finalimage, paldata[0], NDS_WIDTH, NDS_HEIGHT, image8bpp, 0);
			lodepng_encode32_file(finaloutputname, finalrgba, NDS_WIDTH, NDS_HEIGHT);
			free(finalrgba);
			
			if(animframes[curframe].offset > lastoffset) {
				lastoffset = animframes[curframe].offset;
				lastparts = *numparts;
			}
		}
		//~ printf("lastoffset %08x lastparts %08x\n", lastoffset, lastparts);
	}
	
	free(finalimage);
	free(tiledata);
	free(paldata);
	free(gfxbuf);
	free(animbuf);
}
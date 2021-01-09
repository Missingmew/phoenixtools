#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "lodepng/lodepng.h"
#include "phoenixgfx.h"

#define PALSIZE4 (2*16)
#define PALSIZE8 (2*256)

#define MODE_GBA 0
#define MODE_NDS 1
#define MODE_PC  2

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

struct screendims {
	signed width, height;
	signed size;
	signed centerx, centery;
};

struct screendims systemdimensions[3] = {
	/* GBA */
	{
		.width = 240, .height = 160,
		.size = 240*160,
		.centerx = 240/2, .centery = 160/2
	},
	/* NDS */
	{
		.width = 256, .height = 192,
		.size = 256*192,
		.centerx = 256/2, .centery = 192/2
	},
	/* PC same as GBA */
	{
		.width = 240, .height = 160,
		.size = 240*160,
		.centerx = 240/2, .centery = 160/2
	},
};

struct tiledatastate {
	/* persistent state, set by hand */
	unsigned mode;
	
	/* dynamic state, changed by setuptiledatastate */
	unsigned statesrc;
	
	unsigned numpals;
	unsigned isrle;
	unsigned char *palbase, *paldata;
	unsigned char *rawtiledata;
	uint16_t **tiledata;
	void *gfxbase;
	uint32_t *partslist;
};

struct animetatile {
	signed x, y;
	signed w, h; /* while never < 0, will make sanitychecks castless */
	unsigned index;
	unsigned twinpalette;
	unsigned basepalette;
};

struct partdesc_gba {
	signed x:8;
	signed y:8;
	unsigned tilenum:11;
	unsigned palnum:1;
	unsigned shape:2;
	unsigned size:2;
}__attribute__((packed));

struct partdesc_nds {
	signed x:8;
	signed y:8;
	unsigned tilenum:8; // was 10
	unsigned pad:1;
	unsigned palnum:2;
	unsigned twinpal:1; /* ? */
	unsigned shape:2;
	unsigned size:2;
}__attribute__((packed));

struct animationhead {
	uint16_t null;
	uint16_t numentries;
	uint32_t tiledataoff;
}__attribute__((packed));

struct animationframe {
	uint16_t offset;
	uint8_t duration;
	uint8_t palmod;
	uint32_t null;
}__attribute__((packed));

struct playfile_data {
	uint32_t anim_offset;
	uint16_t unk4;
	uint16_t unk6;
	char name[8];
	uint32_t null;
}__attribute__((packed));

void part2animeta(struct animetatile *dst, void *src, unsigned mode) {
	unsigned pad = 0;
	switch(mode) {
		case MODE_PC:
		case MODE_GBA: {
			struct partdesc_gba *partsrc = src;
			dst->x = partsrc->x;
			dst->y = partsrc->y;
			dst->w = sizelut[partsrc->shape][partsrc->size].w;
			dst->h = sizelut[partsrc->shape][partsrc->size].h;
			dst->index = partsrc->tilenum;
			dst->twinpalette = 0;
			dst->basepalette = partsrc->palnum;
			break;
		}
		case MODE_NDS: {
			struct partdesc_nds *partsrc = src;
			dst->x = partsrc->x;
			dst->y = partsrc->y;
			dst->w = sizelut[partsrc->shape][partsrc->size].w;
			dst->h = sizelut[partsrc->shape][partsrc->size].h;
			dst->index = partsrc->tilenum;
			dst->twinpalette = partsrc->twinpal;
			dst->basepalette = partsrc->palnum;
			pad = partsrc->pad;
			break;
		}
		default: {
			printf("%s: bad mode %u\n", __func__, mode);
			return;
		}
	}
	//~ printf("loaded part: x=%4d(%3d) y=%4d(%3d) twinpal=%u palnum=%u w=%2u h=%2u tilenum=%4u\n", dst->x, dst->x+systemdimensions[mode].centerx, dst->y, dst->y+systemdimensions[mode].centery, dst->twinpalette, dst->basepalette, dst->w, dst->h, dst->index);
	if(pad) printf("part has nonzero pad!\n");
}

unsigned setuptiledatastate(struct tiledatastate *dst, void *src, unsigned offset) {
	src += offset;
	/* msb denotes wether graphics data is plain or RLE with index table */
	dst->numpals = *((uint32_t *)src) & 0x7FFFFFFF;
	dst->isrle = *((uint32_t *)src) & 0x80000000;
	if(dst->numpals > 16) {
		printf("%s: was about to attempt to load > 16 palettes (%u)!\n", __func__, dst->numpals);
		return 0;
	}
	if(!dst->paldata) dst->paldata = malloc(0x100*2);
	memset(dst->paldata, 0, 0x100*2);
	dst->palbase = src+4;
	memcpy(dst->paldata, dst->palbase, dst->numpals * 0x10*2);
	if(dst->isrle) {
		dst->partslist = dst->gfxbase = dst->palbase+dst->numpals*0x20;
		if(dst->tiledata) {
			free(dst->tiledata);
			dst->tiledata = 0;
		}
		dst->tiledata = malloc((dst->partslist[0] / 4)*sizeof(uint16_t *));
		for(unsigned i = 0; i < (dst->partslist[0] / 4); i++) dst->tiledata[i] = dst->gfxbase+dst->partslist[i];
		printf("%s: graphics file has %u palettes and %u metatiles\n", __func__, dst->numpals, dst->partslist[0] / 4);
	}
	else {
		if(dst->tiledata) {
			free(dst->tiledata);
			dst->tiledata = 0;
		}
		dst->rawtiledata = dst->gfxbase = dst->palbase+dst->numpals*0x20;
		printf("%s: graphics file has %u palettes\n", __func__, dst->numpals);
	}
	dst->statesrc = offset;
	return 1;
}

void copynonzero(unsigned char *dest, unsigned char *src, unsigned count) {
	for(unsigned i = 0; i < count; i++, src++, dest++) {
		if(*src) *dest = *src;
	}
}

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
void blit_metatile(unsigned char *dest, struct animetatile *meta, unsigned bpp, struct tiledatastate *state) {
	unsigned tilesize;
	unsigned char *blitdest = NULL, *blitsrc = NULL, *srctiles = NULL, *srclinear = NULL;
	unsigned xofs = 0, yofs = 0;
	unsigned xlimit = 0, ylimit = 0;
	
	unsigned metawtiles, metahtiles;
	metawtiles = meta->w/8;
	metahtiles = meta->h/8;
	
	if(bpp == image4bpp) tilesize = 32;
	else if(bpp == image8bpp) tilesize = 64;
	else {
		printf("%s: unknown bpp %u\n", __func__, bpp);
		return;
	}
	
	//~ printf("meta: x=%d y=%d w=%u(%u) h=%u(%u) idx %u\n", meta->x, meta->y, meta->w, metawtiles, meta->h, metahtiles, meta->index);
	//~ printf("%u %u %u %u\n", meta->x+meta->w < 0, meta->x >= systemdimensions[state->mode].width, meta->y+meta->h < 0, meta->y >= systemdimensions[state->mode].height);
	/* check if there is anything to be drawn within bounds */
	if(meta->x+meta->w < 0) return;
	if(meta->x >= systemdimensions[state->mode].width) return;
	if(meta->y+meta->h < 0) return;
	if(meta->y >= systemdimensions[state->mode].height) return;
	
	/* adjust x and y offset to be within screen bounds, skip everything outside */
	if(meta->x < 0) xofs = meta->x * -1;
	if(meta->x+meta->w >= systemdimensions[state->mode].width) xlimit = meta->x+meta->w - systemdimensions[state->mode].width;
	
	if(meta->y < 0) yofs = meta->y * -1;
	if(meta->y+meta->h >= systemdimensions[state->mode].height) ylimit = meta->y+meta->h - systemdimensions[state->mode].height;
	
	/* decompress and convert to linear to allow easy off-grid printing */
	if(state->isrle) srctiles = deRLE(state->tiledata[meta->index], metawtiles*metahtiles*tilesize);
	// TEST THIS
	else srctiles = state->rawtiledata+(tilesize*meta->index);
	
	srclinear = generateIndexedImageFromTiles(srctiles, metawtiles, metahtiles, bpp, NULL);
	
	if(meta->basepalette) {
		for(unsigned i = 0; i < meta->w*meta->h; i++) if(srclinear[i]) srclinear[i] += 0x10*meta->basepalette;
	}
	
	if(meta->twinpalette && bpp == image4bpp) {
		for(unsigned i = 0; i < meta->w*meta->h; i++) if(srclinear[i]) srclinear[i] += 0x10*meta->twinpalette;
	}
	
	blitdest = dest + meta->x+xofs + systemdimensions[state->mode].width*(meta->y+yofs);
	blitsrc = srclinear + xofs + meta->w*yofs;
	
	//~ printf("dest %p src %p meta %p imw %u bpp %u\n", dest, src, meta, systemdimensions[state->mode].width, bpp);
	//~ printf("tilesize %u blitdest %p blitsrc %p (offset by %d)\n", tilesize, blitdest, blitsrc, meta->x + systemdimensions[state->mode].width*meta->y);
	//~ printf("xofs=%u xlimit=%u yofs=%u ylimit=%u\n", xofs, xlimit, yofs, ylimit);
	
	for(unsigned h = 0; h < meta->h-yofs-ylimit; h++) {
		copynonzero(blitdest+h*systemdimensions[state->mode].width, blitsrc+h*meta->w, meta->w-xofs-xlimit);
	}
	
	if(state->isrle) free(srctiles);
	free(srclinear);
}

unsigned isvalidtable(struct animationhead *head) {
	/* what i assumed to be padding is actually the offset into the tile data used in plain mode
	   so theres not much left to validate :/ */
	return !(head->null);
}

unsigned extractframe(unsigned char *dst, uint32_t *src, struct tiledatastate *state) {
	uint32_t numparts;
	uint32_t *partheads = NULL;
	numparts = *src;
	partheads = (void *)src + sizeof(uint32_t);
	
	for(unsigned curpart = 0; curpart < numparts; curpart++) {
		struct animetatile metatile;
		part2animeta(&metatile, &partheads[curpart], state->mode);
		metatile.twinpalette *= (state->numpals/2);
		
		/* convert relative positions to absolute ones */
		metatile.x += systemdimensions[state->mode].centerx;
		metatile.y += systemdimensions[state->mode].centery;
		blit_metatile(dst, &metatile, image4bpp, state);
	}
	return numparts;
}

int main(int argc, char **argv) {
	FILE *gfx = NULL, *anim = NULL, *play = NULL;
	char finaloutputname[256];
	
	unsigned gfxsize, animsize;
	void *gfxbuf = NULL, *animbuf = NULL;
	
	struct tiledatastate state = {0};
	
	unsigned numtotalframes;
	struct animationhead *animhead = NULL;
	struct animationframe *animframes = NULL;
	unsigned extractedpartcount = 0;
	
	unsigned char *finalimage = NULL;
	unsigned char *finalrgba = NULL;
	
	/* PC PLY stuff */
	uint16_t numgfxblocks;
	uint16_t numanimations;
	uint32_t *gfxoffsets = NULL;
	struct playfile_data *plydat = NULL;
	
	
	if(argc < 5) {
		printf("not enough args! use %s mode graphics animation outputprefix/PLY-file\n", argv[0]);
		printf("allowed modes are: 'g' for GBA, 'p' for the native PC port and 'n' for NDS\n");
		printf("in mode p the last argument must be a PLY file, else it must be the prefix for dumped files\n");
		return 1;
	}
	
	switch(*argv[1]) {
		case 'g': {
			printf("mode GBA\n");
			state.mode = MODE_GBA;
			break;
		}
		case 'n': {
			printf("mode NDS\n");
			state.mode = MODE_NDS;
			break;
		}
		case 'p': {
			printf("mode PC\n");
			state.mode = MODE_PC;
			break;
		}
		default: {
			printf("unknown mode %c\n", *argv[1]);
			return 1;
		}
	}
	
	if(!(gfx = fopen(argv[2], "rb"))) {
		printf("couldnt open %s for reading\n", argv[2]);
		return 1;
	}
	fseek(gfx, 0, SEEK_END);
	gfxsize = ftell(gfx);
	fseek(gfx, 0, SEEK_SET);
	gfxbuf = malloc(gfxsize);
	fread(gfxbuf, gfxsize, 1, gfx);
	fclose(gfx);
	
	if(!(anim = fopen(argv[3], "rb"))) {
		printf("couldnt open %s for reading\n", argv[3]);
		return 1;
	}
	fseek(anim, 0, SEEK_END);
	animsize = ftell(anim);
	fseek(anim, 0, SEEK_SET);
	animbuf = malloc(animsize);
	fread(animbuf, animsize, 1, anim);
	fclose(anim);
	
	setuptiledatastate(&state, gfxbuf, 0);
	
	finalimage = malloc(systemdimensions[state.mode].size);
	
	numtotalframes = 0;
	
	
	if(state.mode == MODE_PC) {
		if(!(play = fopen(argv[4], "rb"))) {
			printf("couldnt open %s for reading\n", argv[4]);
			return 1;
		}
		fread(&numgfxblocks, sizeof(uint16_t), 1, play);
		fread(&numanimations, sizeof(uint16_t), 1, play);
		gfxoffsets = malloc(numgfxblocks*sizeof(uint32_t));
		plydat = malloc(numanimations*sizeof(struct playfile_data));
		fread(gfxoffsets, numgfxblocks*sizeof(uint32_t), 1, play);
		fread(plydat, numanimations*sizeof(struct playfile_data), 1, play);
		fclose(play);
		printf("PLY-file lists the following graphics block addresses:\n");
		for(unsigned i = 0; i < numgfxblocks; i++) {
			uint32_t *gfxhead = gfxbuf+gfxoffsets[i];
			printf("%08x: has %d palettes, RLE is %d\n", gfxoffsets[i], *gfxhead & 0x7FFFFFFF, !!(*gfxhead & 0x80000000));
		}
		printf("\nPLY-file lists the following animations:\n");
		for(unsigned i = 0; i < numanimations; i++) {
			animhead = animbuf + plydat[i].anim_offset;
			printf("%08x (%04x %04x): %.8s (%x), animation has %d entries, graphicsoffset is %08x (%x)\n", plydat[i].anim_offset, plydat[i].unk4, plydat[i].unk6, plydat[i].name, plydat[i].null, animhead->numentries, animhead->tiledataoff, animhead->null);
		}
		
		/* HORRIBLE COPYPASTA ARGH */
		unsigned curframe = 0;
		for(unsigned i = 0; i < numanimations; i++) {
			animhead = animbuf + plydat[i].anim_offset;
			if(animhead->tiledataoff != state.statesrc) {
				if(animhead->tiledataoff > gfxsize) {
					printf("Animation %.8s has invalid tiledatasource %08x gfxsize is %08x\n", plydat[i].name, animhead->tiledataoff, gfxsize);
					break;
				}
				printf("Animation %.8s has new tiledatasource, loading (was %08x, new %08x)\n", plydat[i].name, state.statesrc, animhead->tiledataoff);
				if(!setuptiledatastate(&state, gfxbuf, animhead->tiledataoff)) {
					break;
				}
			}
			animframes = (void *)animhead+sizeof(struct animationhead);
			printf("Animation %.8s at %08x has %u animations\n", plydat[i].name, plydat[i].anim_offset, animhead->numentries);
			numtotalframes += animhead->numentries;
			
			for(curframe = 0; curframe < animhead->numentries; curframe++) {
				memset(finalimage, 0, systemdimensions[state.mode].size);
				
				extractedpartcount = extractframe(finalimage, (void *)animhead + animframes[curframe].offset, &state);
				
				printf("Frame %2u dataoff %08x(abs %08lx) loaded %u parts\n", curframe, animframes[curframe].offset, (unsigned char *)animhead + animframes[curframe].offset-(unsigned char *)animbuf, extractedpartcount);
				
				sprintf(finaloutputname, "%.8s-fr%02u.png", plydat[i].name, curframe);
				finalrgba = linearImageWithPaletteToRGBA(finalimage, state.paldata, systemdimensions[state.mode].width, systemdimensions[state.mode].height, image8bpp, 0);
				lodepng_encode32_file(finaloutputname, finalrgba, systemdimensions[state.mode].width, systemdimensions[state.mode].height);
				free(finalrgba);
				
			}
		}
		
		free(plydat);
		free(gfxoffsets);
		goto cleanup;
	}
	
	/* curframe needs to be out here to survive loop iterations for extra frame naming */
	unsigned currenttable, endoftable, lastoffset, lastparts, curframe = 0;
	for(currenttable = 0, endoftable = 0; endoftable+sizeof(struct animationhead)-1 < animsize; endoftable += lastoffset + lastparts * sizeof(uint32_t) + sizeof(uint32_t), currenttable++) {
		lastoffset = 0;
		lastparts = 0;
		animhead = animbuf+endoftable;
		if(!isvalidtable(animhead)) {
			/* only when last table */
			if(endoftable+sizeof(struct animationhead) >= animsize) break;
			
			printf("Table %2u has extra frames after the last referenced one! First unreferenced data at %08x\n", currenttable-1, endoftable);
			if(*(uint32_t *)(animbuf+endoftable) < 64) { // very crude heuristic :/
				// dump this extra data as a frame
				/* BEGIN COPYPASTA */
				memset(finalimage, 0, systemdimensions[state.mode].size);
				
				extractedpartcount = extractframe(finalimage, animbuf+endoftable, &state);
				
				printf("Extraframe loaded %u parts\n", extractedpartcount);
				// one ugh housekeeping, make sure extra frame counts towards the table of the earlier ones
				currenttable--;
				
				sprintf(finaloutputname, "%s-tb%02u-fr%02u.png", argv[4], currenttable, curframe);
				finalrgba = linearImageWithPaletteToRGBA(finalimage, state.paldata, systemdimensions[state.mode].width, systemdimensions[state.mode].height, image8bpp, 0);
				lodepng_encode32_file(finaloutputname, finalrgba, systemdimensions[state.mode].width, systemdimensions[state.mode].height);
				free(finalrgba);
				
				// more ugh housekeeping, advance frame counter for possibly following extra frames, adjust lastoffset and lastparts to skip over this extra frame
				curframe++;
				lastoffset = 0;
				lastparts = extractedpartcount;
				// skip remaining code, its possible there may be more extra frames after this one
				continue;
				
				/* END COPYPASTA */
			}
			/* this used to search for new usable tables. dumping extraframes results in the same
			while(endoftable+sizeof(struct animationhead)-1 < animsize && !isvalidtable(animhead)) {
				endoftable += 4;
				animhead = animbuf+endoftable;
			}
			*/
		}
		
		if(animhead->tiledataoff != state.statesrc) {
			if(animhead->tiledataoff > gfxsize) {
				printf("Table %2u has invalid tiledatasource %08x gfxsize is %08x\n", currenttable, animhead->tiledataoff, gfxsize);
				currenttable--;
				break;
			}
			printf("Table %2u has new tiledatasource, loading (was %08x, new %08x)\n", currenttable, state.statesrc, animhead->tiledataoff);
			if(!setuptiledatastate(&state, gfxbuf, animhead->tiledataoff)) {
				currenttable--;
				break;
			}
		}
		animframes = (void *)animhead+sizeof(struct animationhead);
		printf("Table %2u at %08x has %u animations\n", currenttable, endoftable, animhead->numentries);
		numtotalframes += animhead->numentries;
		
		for(curframe = 0; curframe < animhead->numentries; curframe++) {
			if(endoftable + animframes[curframe].offset >= animsize) {
				printf("Table %2u entry %u points to EOF!\n", currenttable, curframe);
				endoftable += animframes[curframe].offset; // make sure to break out of the outer loop
				break;
			}
			memset(finalimage, 0, systemdimensions[state.mode].size);
			
			extractedpartcount = extractframe(finalimage, (void *)animhead + animframes[curframe].offset, &state);
			
			printf("Frame %2u dataoff %08x(abs %08lx) loaded %u parts\n", curframe, animframes[curframe].offset, (unsigned char *)animhead + animframes[curframe].offset-(unsigned char *)animbuf, extractedpartcount);
			
			sprintf(finaloutputname, "%s-tb%02u-fr%02u.png", argv[4], currenttable, curframe);
			finalrgba = linearImageWithPaletteToRGBA(finalimage, state.paldata, systemdimensions[state.mode].width, systemdimensions[state.mode].height, image8bpp, 0);
			lodepng_encode32_file(finaloutputname, finalrgba, systemdimensions[state.mode].width, systemdimensions[state.mode].height);
			free(finalrgba);
			
			if(animframes[curframe].offset > lastoffset) {
				lastoffset = animframes[curframe].offset;
				lastparts = extractedpartcount;
			}
		}
		//~ printf("lastoffset %08x lastparts %08x\n", lastoffset, lastparts);
	}
	printf("Found a total of %2u tables with %u animations\n", currenttable, numtotalframes);
	
	cleanup:
	if(finalimage) free(finalimage);
	if(state.tiledata) free(state.tiledata);
	if(state.paldata) free(state.paldata);
	if(gfxbuf) free(gfxbuf);
	if(animbuf) free(animbuf);
}

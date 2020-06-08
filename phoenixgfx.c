#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "phoenixgfx.h"

#define color5to8(x) ((x*8)+(x/4))
#define alpha3to8(x) color5to8(((x*4)+(x/2)))
#define tileside 8
#define tilesize8 (tileside*tileside)
#define tilesize4 ((tileside*tileside)/2)

static inline unsigned int getPixelInTile8(unsigned int x, unsigned int y, unsigned int hflip, unsigned int vflip) {
	if(hflip) {
		if(vflip) return (7-y)*tileside+(7-x);
		else return (7-y)*tileside+x;
	}
	else {
		if(vflip) return y*tileside+(7-x);
		else return y*tileside+x;
	}
	return 0;
}

static inline unsigned int getPixelInTile4(unsigned int x, unsigned int y, unsigned int hflip, unsigned int vflip) {
	if(hflip) {
		if(vflip) return (7-y)*(tileside/2)+(7-x)/2;
		else return (7-y)*(tileside/2)+x/2;
	}
	else {
		if(vflip) return y*(tileside/2)+(7-x)/2;
		else return y*(tileside/2)+x/2;
	}
	return 0;
}

typedef struct {
	uint32_t tilenum;
	uint32_t vflip;
	uint32_t hflip;
	uint32_t palnum;
}tilemapEntry;

/*	generates palettes from RGB555 data
	NOTE: the returned uint32_t is in the following order in memory:
	AABBGGRR, NOT in BBGGRRAA
	*/
	
uint32_t *generatePalette(unsigned char *source, unsigned int bpp) {
	int i;
	uint16_t curentry;
	uint8_t red, green, blue;
	uint32_t *retpal = NULL;
	switch(bpp) {
		case image4bpp: {
			retpal = malloc(sizeof(uint32_t)*16);
			for(i = 0; i < 16; i++) {
				curentry = source[i*2] | source[(i*2)+1] << 8;
				red = curentry & 0x1F;
				green = (curentry >> 5) & 0x1F;
				blue = (curentry >> 10) & 0x1F;
				retpal[i] = 0xFF000000 | color5to8(red) | color5to8(green) << 8 | color5to8(blue) << 16;
			}
			break;
		}
		case image8bpp: {
			retpal = malloc(sizeof(uint32_t)*256);
			for(i = 0; i < 256; i++) {
				curentry = source[i*2] | source[(i*2)+1] << 8;
				red = curentry & 0x1F;
				green = (curentry >> 5) & 0x1F;
				blue = (curentry >> 10) & 0x1F;
				retpal[i] = 0xFF000000 | color5to8(red) | color5to8(green) << 8 | color5to8(blue) << 16;
			}
			break;
		}
		case image2bpp: {
			retpal = malloc(sizeof(uint32_t)*4);
			for(i = 0; i < 4; i++) {
				curentry = source[i*2] | source[(i*2)+1] << 8;
				red = curentry & 0x1F;
				green = (curentry >> 5) & 0x1F;
				blue = (curentry >> 10) & 0x1F;
				retpal[i] = 0xFF000000 | color5to8(red) | color5to8(green) << 8 | color5to8(blue) << 16;
			}
			break;
		}
		case imagea3i5: {
			retpal = malloc(sizeof(uint32_t)*32);
			for(i = 0; i < 32; i++) {
				curentry = source[i*2] | source[(i*2)+1] << 8;
				red = curentry & 0x1F;
				green = (curentry >> 5) & 0x1F;
				blue = (curentry >> 10) & 0x1F;
				retpal[i] = 0xFF000000 | color5to8(red) | color5to8(green) << 8 | color5to8(blue) << 16;
			}
			break;
		}
		case imagea5i3: {
			retpal = malloc(sizeof(uint32_t)*8);
			for(i = 0; i < 8; i++) {
				curentry = source[i*2] | source[(i*2)+1] << 8;
				red = curentry & 0x1F;
				green = (curentry >> 5) & 0x1F;
				blue = (curentry >> 10) & 0x1F;
				retpal[i] = 0xFF000000 | color5to8(red) | color5to8(green) << 8 | color5to8(blue) << 16;
			}
			break;
		}
		default: {
			printf("nein palette\n");
			return 0;
		}
	}
	
	return retpal;
}

/*	generates a tilemapEntry list from a tilemap of any kind
	*/
tilemapEntry *parseTilemap(void *tilemap, unsigned int maptype, unsigned int mapsize) {
	unsigned int i;
	tilemapEntry *retlist = malloc(sizeof(tilemapEntry)*mapsize);
	if(maptype) { /* uint16_t */
		uint16_t *map16 = (uint16_t *)tilemap;
		for(i=0;i<mapsize;i++) {
			retlist[i].tilenum = map16[i] & 0x3FF; /* bit 0-9 */
			retlist[i].vflip = (map16[i] & (1<<10))?1:0;
			retlist[i].hflip = (map16[i] & (1<<11))?1:0;
			// retlist[i].palnum = map16[i] >> 12; /* bit 12-15 */
			retlist[i].palnum = 0;
			// printf("index %04x - num %04x - flip hv %d%d - palnum %d\n", i, retlist[i].tilenum, retlist[i].hflip, retlist[i].vflip, retlist[i].palnum);
		}	
	}
	else { /* uint8_t */
		uint8_t *map8 = (uint8_t *)tilemap;
		for(i=0;i<mapsize;i++) {
			retlist[i].tilenum = map8[i];
			retlist[i].hflip = 0;
			retlist[i].vflip = 0;
			retlist[i].palnum = 0;
		}
	}
	return retlist;
}

/*	generates indexed image data from indexed tile data
	if no tilemap is available, pass NULL
	*/
	
uint8_t *generateIndexedImageFromTiles(unsigned char *source, unsigned int tilesWidth, unsigned int tilesHeight, unsigned int bpp, tilemapEntry *tilemap) {
	uint8_t *retidx = malloc( tilesWidth*tilesHeight*tilesize8 );
	unsigned int x, y, i, j, t = 0;
	unsigned int tilepos;
	if(tilemap) {
		if( bpp == image4bpp ) {
			uint8_t pixelA, pixelB;
			for( y = 0; y < tilesHeight; y++ ) {			// height tiles
				for( x = 0; x < tilesWidth; x++ ) {			// width tiles
					tilepos = x+(tilesWidth*y);
					for( i = 0; i < tileside; i++ ) {		// per tile pixel height
						for( j = 0; j < tileside; j++ ) {	// per tile pixel width
							pixelA = source[(tilemap[tilepos].tilenum*(tilesize4))+getPixelInTile4(j, i, tilemap[tilepos].hflip, tilemap[tilepos].vflip)] & 0x0F;
							pixelB = source[(tilemap[tilepos].tilenum*(tilesize4))+getPixelInTile4(j, i, tilemap[tilepos].hflip, tilemap[tilepos].vflip)] >> 4;
							if(tilemap[tilepos].vflip) {
								retidx[((x*tileside)+j)+(((y*tileside)+i)*(tilesWidth*tileside))] = pixelB;
								j++;
								retidx[((x*tileside)+j)+(((y*tileside)+i)*(tilesWidth*tileside))] = pixelA;
							}
							else {
								retidx[((x*tileside)+j)+(((y*tileside)+i)*(tilesWidth*tileside))] = pixelA;
								j++;
								retidx[((x*tileside)+j)+(((y*tileside)+i)*(tilesWidth*tileside))] = pixelB;
							}
						}
					}
				}
			}
		}
		else if( bpp == image8bpp ) {
			for( y = 0; y < tilesHeight; y++ ) {			// height tiles
				for( x = 0; x < tilesWidth; x++ ) {			// width tiles
					tilepos = x+(tilesWidth*y);
					for( i = 0; i < tileside; i++ ) {		// per tile pixel height
						for( j = 0; j < tileside; j++ ) {	// per tile pixel width
							retidx[((x*tileside)+j)+(((y*tileside)+i)*(tilesWidth*tileside))] = source[(tilemap[tilepos].tilenum*(tilesize8))+getPixelInTile8(j, i, tilemap[tilepos].hflip, tilemap[tilepos].vflip)];
							t++;
						}
					}
				}
			}
		}
	}
	else {
		if( bpp == image4bpp ) {
			uint8_t pixelA, pixelB;
			for( y = 0; y < tilesHeight; y++ ) {			// height tiles
				for( x = 0; x < tilesWidth; x++ ) {			// width tiles
					for( i = 0; i < tileside; i++ ) {		// per tile pixel height
						for( j = 0; j < tileside; j++ ) {	// per tile pixel width
							pixelA = source[t] & 0x0F;
							pixelB = source[t] >> 4;
							retidx[((x*tileside)+j)+(((y*tileside)+i)*(tilesWidth*tileside))] = pixelA;
							j++;
							retidx[((x*tileside)+j)+(((y*tileside)+i)*(tilesWidth*tileside))] = pixelB;
							t++;
							/*
							posx = (x * tilesize) + j;
							posy = ((y * tilesize) + i ) * (tilesWidth * tilesize)
							*/
						}
					}
				}
			}
		}
		else if( bpp == image8bpp ) {
			for( y = 0; y < tilesHeight; y++ ) {			// height tiles
				for( x = 0; x < tilesWidth; x++ ) {			// width tiles
					for( i = 0; i < tileside; i++ ) {		// per tile pixel height
						for( j = 0; j < tileside; j++ ) {	// per tile pixel width
							retidx[((x*tileside)+j)+(((y*tileside)+i)*(tilesWidth*tileside))] = source[t];
							t++;
						}
					}
				}
			}
		}
	}
	return retidx;
}

/*	generates 8bpp indexed image data from any other indexed image data
	*/

uint8_t *generate8bppIndexedFromAnyBpp(unsigned char *source, unsigned int totalPixels, unsigned int bpp) {
	uint8_t *retidx = malloc(totalPixels);
	unsigned int i, p = 0;
	switch(bpp) {
		case image4bpp: {
			uint8_t pixelA, pixelB;
			for(i = 0; i < totalPixels/2; i++) {
				pixelA = source[i] & 0x0F;
				pixelB = source[i] >> 4;
				retidx[p] = pixelA;
				p++;
				retidx[p] = pixelB;
				p++;
			}
			break;
		}
		case image2bpp: {
			uint8_t pixelA, pixelB, pixelC, pixelD;
			for( i = 0; i < totalPixels/4; i++ ) {
				pixelA = source[i] & 0x03;
				pixelB = (source[i] >> 2) & 0x03;
				pixelC = (source[i] >> 4) & 0x03;
				pixelD = (source[i] >> 6) & 0x03;
				retidx[p] = pixelA;
				p++;
				retidx[p] = pixelB;
				p++;
				retidx[p] = pixelC;
				p++;
				retidx[p] = pixelD;
				p++;
			}
			break;
		}
		default: {
			printf("nein gen8idx\n");
			free(retidx);
			return 0;
		}
	}
				
	return retidx;
}

/*	generates RGBA image from tiled image data
	source must be uncompressed */	

unsigned char *tiledImageToRGBA(unsigned char *source, unsigned int tilesWidth, unsigned int tilesHeight, unsigned int bpp, unsigned int hasAlpha) {
	unsigned char *imagedata = source + ((bpp == image8bpp)?512:32);
	unsigned int i, numPixels = (tilesWidth*8)*(tilesHeight*8);
	uint32_t *palette = generatePalette(source, bpp);
	if( hasAlpha ) palette[0] &= 0x00FFFFFF;
	uint8_t  *idximage = generateIndexedImageFromTiles(imagedata, tilesWidth, tilesHeight, bpp, NULL);
	uint32_t *retimage = malloc(sizeof(uint32_t)*numPixels);
	for( i = 0; i < numPixels; i++ ) {
		retimage[i] = palette[idximage[i]];
	}
	free(palette);
	free(idximage);
	return (unsigned char*)retimage;
}

/*	generates RGBA image from striped image data
	source must be uncompressed
	uncompress all tile strips one after another in one array to get the complete image
	can also be used to obtain RGBA from patches
	simply pass the palette of the original image as paletteData */

unsigned char *tiledImageWithPaletteToRGBA(unsigned char *source, unsigned char *paletteData, unsigned int tilesWidth, unsigned int tilesHeight, unsigned int bpp, unsigned int hasAlpha) {
	unsigned int i, numPixels = (tilesWidth*8)*(tilesHeight*8);
	uint32_t *palette = generatePalette(paletteData, bpp);
	if( hasAlpha ) palette[0] &= 0x00FFFFFF;
	uint8_t  *idximage = generateIndexedImageFromTiles(source, tilesWidth, tilesHeight, bpp, NULL);
	uint32_t *retimage = malloc(sizeof(uint32_t)*numPixels);
	for( i = 0; i < numPixels; i++ ) {
		retimage[i] = palette[idximage[i]];
	}
	free(palette);
	free(idximage);
	return (unsigned char*)retimage;
}

/*	generates RGBA image from linear image data
	source must be uncompressed */
	
unsigned char *linearImageWithPaletteToRGBA(unsigned char *source, unsigned char *paletteData, unsigned int pixelsWidth, unsigned int pixelsHeight, unsigned int bpp, unsigned int hasAlpha) {
	unsigned int i, numPixels = pixelsWidth*pixelsHeight;
	uint8_t *idximage, alpha, color;
	uint32_t *palette = generatePalette(paletteData, bpp);
	if( hasAlpha ) palette[0] &= 0x00FFFFFF;
	if( bpp == (image4bpp || image2bpp) ) idximage = generate8bppIndexedFromAnyBpp(source, numPixels, bpp);
	else idximage = source;
	uint32_t *retimage = malloc(sizeof(uint32_t)*numPixels);
	switch(bpp) {
		case image2bpp:
		case image4bpp:
		case image8bpp: {
			for( i = 0; i < numPixels; i++ ) retimage[i] = palette[idximage[i]];
			break;
		}
		case imagea3i5: {
			for( i = 0; i < numPixels; i++ ) {
				alpha = alpha3to8(idximage[i] >> 5);
				color = idximage[i] & 0x1F;
				retimage[i] = (palette[color] & 0x00FFFFFF) | alpha << 24;
			}
			break;
		}
		case imagea5i3: {
			for( i = 0; i < numPixels; i++ ) {
				alpha = color5to8(idximage[i] >> 3);
				color = idximage[i] & 0x07;
				retimage[i] = (palette[color] & 0x00FFFFFF) | alpha << 24;
			}
			break;
		}
		default: {
			printf("nein linearimage\n");
			free(palette);
			free(retimage);
			return 0;
		}
	}
	free(palette);
	if( bpp == image4bpp ) free(idximage);
	return (unsigned char*)retimage;
}

/*	generates RGBA image from tiled image data and tilemap
	note that mapsize is in number of entries, not number of bytes */

unsigned char *tiledImageWithPaletteAndTilesetToRGBA(unsigned char *source, unsigned char *paletteData, void *tilemap, unsigned int tilesWidth, unsigned int tilesHeight, unsigned int bpp, unsigned int hasAlpha, unsigned int maptype, unsigned int mapsize) {
	unsigned int i, numPixels = (tilesWidth*8)*(tilesHeight*8), tileindex = 0;
	uint32_t *palette = generatePalette(paletteData, bpp);
	if( hasAlpha ) palette[0] &= 0x00FFFFFF;
	tilemapEntry *workmap = parseTilemap(tilemap, maptype, mapsize);
	uint8_t  *idximage = generateIndexedImageFromTiles(source, tilesWidth, tilesHeight, bpp, workmap);
	uint32_t *retimage = malloc(sizeof(uint32_t)*numPixels);
	if( bpp == image4bpp ) {
		for( i = 0; i < numPixels; i++ ) {
			if( i != 0 && !(i%tilesize8) ) tileindex++;
			retimage[i] = palette[idximage[i]+(32*workmap[tileindex].palnum)];
		}
	}
	else {
		for( i = 0; i < numPixels; i++ ) {
			retimage[i] = palette[idximage[i]];
		}
	}
	free(palette);
	free(workmap);
	free(idximage);
	return (unsigned char*)retimage;
}

#ifndef PHOENIXGFX_H_
#define PHOENIXGFX_H_

enum gfxmode {
	image2bpp,
	image4bpp,
	image8bpp,
	imagexbpp,
	imagea3i5,
	imagea5i3
};

typedef struct {
	uint32_t tilenum;
	uint32_t vflip;
	uint32_t hflip;
	uint32_t palnum;
}tilemapEntry;

/* 	phoenixgfx only functions, should not be called from outside
	uint32_t *generatePalette(unsigned char *source, unsigned int bpp);
	uint8_t *generate8bppIndexedFromAnybpp(unsigned char *source, unsigned int totalPixels);
	*/
uint8_t *generateIndexedImageFromTiles(unsigned char *source, unsigned int tilesWidth, unsigned int tilesHeight, unsigned int bpp, tilemapEntry *tilemap);
unsigned char *tiledImageToRGBA(unsigned char *source, unsigned int tilesWidth, unsigned int tilesHeight, unsigned int bpp, unsigned int hasAlpha);
unsigned char *tiledImageWithPaletteToRGBA(unsigned char *source, unsigned char *paletteData, unsigned int tilesWidth, unsigned int tilesHeight, unsigned int bpp, unsigned int hasAlpha);
unsigned char *linearImageWithPaletteToRGBA(unsigned char *source, unsigned char *paletteData, unsigned int pixelsWidth, unsigned int pixelsHeight, unsigned int bpp, unsigned int hasAlpha);
unsigned char *tiledImageWithPaletteAndTilesetToRGBA(unsigned char *source, unsigned char *paletteData, void *tilemap, unsigned int tilesWidth, unsigned int tilesHeight, unsigned int bpp, unsigned int hasAlpha, unsigned int maptype, unsigned int mapsize);
#endif

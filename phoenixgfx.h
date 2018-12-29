enum gfxmode{
	image2bpp,
	image4bpp,
	image8bpp,
	imagexbpp,
	imagea3i5,
	imagea5i3
};

/* 	phoenixgfx only functions, should not be called from outside
	uint32_t *generatePalette(unsigned char *source, unsigned int bpp);
	uint8_t *generateIndexedImageFromTiles(unsigned char *source, unsigned int tilesWidth, unsigned int tilesHeight, unsigned int bpp, tilemapEntry *tilemap);
	uint8_t *generate8bppIndexedFromAnybpp(unsigned char *source, unsigned int totalPixels);
	*/
unsigned char *tiledImageToRGBA(unsigned char *source, unsigned int tilesWidth, unsigned int tilesHeight, unsigned int bpp, unsigned int hasAlpha);
unsigned char *tiledImageWithPaletteToRGBA(unsigned char *source, unsigned char *paletteData, unsigned int tilesWidth, unsigned int tilesHeight, unsigned int bpp, unsigned int hasAlpha);
unsigned char *linearImageWithPaletteToRGBA(unsigned char *source, unsigned char *paletteData, unsigned int pixelsWidth, unsigned int pixelsHeight, unsigned int bpp, unsigned int hasAlpha);
unsigned char *tiledImageWithPaletteAndTilesetToRGBA(unsigned char *source, unsigned char *paletteData, void *tilemap, unsigned int tilesWidth, unsigned int tilesHeight, unsigned int bpp, unsigned int hasAlpha, unsigned int maptype, unsigned int mapsize);

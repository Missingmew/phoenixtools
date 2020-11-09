#define xprintf(...)
//~ #define xprintf printf
#define xfprintf(...)
//~ #define xfprintf fprintf

unsigned char *unpackBuffer(unsigned char *source, unsigned int *resultsize, unsigned int *compressedsize);

unsigned char *LZ10Decompress(unsigned char *src, int *compressedSize, int *uncompressedSize);
unsigned char *LZ11Decompress(unsigned char *src, int *compressedSize, int *uncompressedSize);

//~ unsigned char *LZXunpack(unsigned char *source, unsigned int destsize, unsigned int *compsize);
//~ unsigned char *LZSSunpack(unsigned char *source, unsigned int destsize, unsigned int *compsize);
//~ unsigned char *HUFFMANunpack(unsigned char *source, unsigned int destsize, unsigned int *compsize, unsigned int mode);
//~ unsigned char *RLEunpack(unsigned char *source, unsigned int destsize, unsigned int *compsize);

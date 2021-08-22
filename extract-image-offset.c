#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "ntrcom/nitrocompression.h"
#include "phoenixgfx.h"
#include "lodepng/lodepng.h"

#define BREAK(x) { printf(x); return 1; }

typedef struct {
	uint32_t offset;
	uint32_t size;
}__attribute__((packed)) pacEntry;

int main( int argc, char** argv ) {
	
	if ( argc < 7 )
	{
		printf( "Not enough arguments given!\nUsage: %s [file] [offset in hex] [type] [tilesx] [tilesy]\n[patch/palette offset/evidence end in hex/bpp for single image/transparency for striped images]\n[source type/transparency]\nFiles will be extracted in current working directory.\nTypes are:\n0 - plain image\n1 - stripped image\n2 - patch\n3 - evidence\n4 - compressed evidence\n5 - Raw image with palette\n6 - AGB striped\nSource types are:\n0 - stripped image\n4/8 - simple image/palette with source type bpp\npatch offset and source type are only needed when extracting a patch/raw image\nbpp and transparency required for single image\nadd 10 to source type for raw images to enable transparency\n", argv[0] );
		return 1;
	}
	
	uint32_t i, numFiles, listOffset, givenOffset, patchOffset, evidenceSize = 2080, agbpaloffset;
	uint32_t *agboffsets;
	unsigned int bpp, resultsize = 0, compressedsize = 0, type, tilesx, tilesy, sourcetype, tempsize = 0, transparency = 0, totalsize;
	givenOffset = strtoul(argv[2], NULL, 16);
	type = strtoul(argv[3], NULL, 10);
	tilesx = strtoul(argv[4], NULL, 10);
	tilesy = strtoul(argv[5], NULL, 10);
	if( type == 0 ) {
		if( argc < 8 ) {
			printf( "missing bpp and or transparency!\nrun without args to see help" );
			return 1;
		}
		else {
			sourcetype = strtoul(argv[6], NULL, 10);
			transparency = strtoul(argv[7], NULL, 10);
		}
	}
	if( type == 1 ) transparency = strtoul(argv[6], NULL, 10);
	if( type == 2 ) {
		if( argc < 8 ) {
			printf( "missing patch offset and or source type!\nrun without args to see help" );
			return 1;
		}
		else {
			patchOffset = strtoul(argv[6], NULL, 16);
			sourcetype = strtoul(argv[7], NULL, 10);
		}
	}
	if(type == 0 && !((sourcetype == 4) || (sourcetype == 8))) {
		printf("unsupported bpp. use either 4 or 8\n");
		return 1;
	}
	else if(type == 2 && !((sourcetype == 0) || (sourcetype == 4) || (sourcetype == 8))) {
		printf("unsupported sourcetype. use either 0, 4 or 8\n");
		return 1;
	}
	if( type == 3 || type == 4 )  {
		if( argc < 7 ) {
			printf( "missing evidence end!\nrun without args to see help" );
			return 1;
		}
		else patchOffset = strtoul(argv[6], NULL, 16);
	}
	if( type == 5 ) {
		if( argc < 8 ) {
			printf( "missing palette offset and or source type!\nrun without args to see help" );
			return 1;
		}
		else {
			patchOffset = strtoul(argv[6], NULL, 16);
			sourcetype = strtoul(argv[7], NULL, 10);
		}
	}
	char outputname[32] = { 0 };
	unsigned char *workbuffer = NULL, *resultbuffer = NULL, *fullbuffer = NULL, *fulltarget = NULL, *rgbaPixelData = NULL, *paletteData = NULL, minibuf[4];
	pacEntry workentry;
	FILE *f;
	
	if( !(f = fopen( argv[1], "rb" ))) {
		printf("Couldnt open file %s\n", argv[1]);
		return 1;
	}
	printf("givenOffset %08x\n", givenOffset);
	fseek( f, givenOffset, SEEK_SET );
	
	switch( type ) {
		case 0: { /* single image */
			bpp = (sourcetype==8)?image8bpp:image4bpp;
			snprintf( outputname, 32, "%08d-single-%08x.png", givenOffset, givenOffset );
			fseek( f, 1, SEEK_CUR );
			fread( &tempsize, 3, 1, f );
			fseek( f, 0, SEEK_END );
			if((givenOffset + 2*tempsize) > ftell(f)) tempsize = ftell(f) - givenOffset;
			else tempsize *= 2;
			fseek( f, givenOffset, SEEK_SET );
			workbuffer = malloc( tempsize );
			fread( workbuffer, tempsize, 1, f );
			compressedsize = tempsize;
			resultbuffer = unpackBuffer( workbuffer, &resultsize, &compressedsize );
			if(bpp == image8bpp) {
				if((tilesx*8*tilesy*8+512) != resultsize) BREAK("size/bpp dont match with uncompressed size\n");
			}
			else if(bpp == image4bpp) {
				if(((tilesx*8*tilesy*8)/2+32) != resultsize) BREAK("size/bpp dont match with uncompressed size\n");
			}
			if( !resultbuffer ) BREAK("error occured\n");
			rgbaPixelData = tiledImageToRGBA(resultbuffer, tilesx, tilesy, bpp, transparency);
			lodepng_encode32_file( outputname, rgbaPixelData, tilesx*8, tilesy*8 );
			free(workbuffer);
			free(resultbuffer);
			free( rgbaPixelData );
			break;
		}
		case 1: { /* striped image */
			fread( &numFiles, 4, 1, f );
			fread( &workentry, sizeof(pacEntry), 1, f );
			listOffset = ftell(f);
			
			paletteData = malloc( workentry.size );
			bpp = (workentry.size == 32)?image4bpp:image8bpp;
			if(bpp == image8bpp) printf("image uses 8bpp\n");
			else if (bpp == image4bpp) printf("image uses 4bpp\n");
			fseek( f, givenOffset + workentry.offset, SEEK_SET );
			fread( paletteData, workentry.size, 1, f );
			
			fseek( f, listOffset, SEEK_SET );
			fullbuffer = malloc(tilesx*8*((numFiles-1)*tilesy)*8);
			fulltarget = fullbuffer;
			for( i = 1; i < numFiles; i++ ) {
				fread( &workentry, sizeof(pacEntry), 1, f );
				listOffset = ftell(f);
				fseek( f, givenOffset + workentry.offset, SEEK_SET );
				
				workbuffer = malloc(workentry.size);
				fread( workbuffer, workentry.size, 1, f );
				compressedsize = workentry.size;
				resultbuffer = unpackBuffer( workbuffer, &resultsize, &compressedsize );
				if( !resultbuffer ) BREAK("error occured\n");
				if(bpp == image8bpp) {
					if((tilesx*8*tilesy*8) != resultsize) BREAK("size/bpp dont match with uncompressed size\n");
				}
				else if(bpp == image4bpp) {
					if(((tilesx*8*tilesy*8)/2) != resultsize) BREAK("size/bpp dont match with uncompressed size\n");
				}
				
				memcpy( fulltarget, resultbuffer, resultsize );
				fulltarget += resultsize;
				free(workbuffer);
				free(resultbuffer);
				if( i == (numFiles-1)) totalsize = ftell(f) - givenOffset;
				fseek( f, listOffset, SEEK_SET );
			}
			rgbaPixelData = tiledImageWithPaletteToRGBA(fullbuffer, paletteData, tilesx, ((numFiles-1)*tilesy), bpp, transparency);
			snprintf( outputname, 32, "%08d-striped-%08x.png", givenOffset, givenOffset );
			lodepng_encode32_file( outputname, rgbaPixelData, tilesx*8, ((numFiles-1)*tilesy)*8 );
			free( fullbuffer );
			free( rgbaPixelData );
			free( paletteData );
			printf("total size of striped image %08x\n", totalsize);
			break;
		}		
		case 2: { /* patch */
			if( !sourcetype ) {
				fread( &numFiles, 4, 1, f );
				fread( &workentry, sizeof(pacEntry), 1, f );
				paletteData = malloc( workentry.size );
				bpp = (workentry.size == 32)?image4bpp:image8bpp;
				fseek( f, givenOffset + workentry.offset, SEEK_SET );
				fread( paletteData, workentry.size, 1, f );
			}
			else {
				bpp = (sourcetype==8)?image8bpp:image4bpp;
				fseek( f, givenOffset + 1, SEEK_SET );
				fread( &tempsize, 3, 1, f );
				fseek( f, 0, SEEK_END );
				if((givenOffset + 2*tempsize) > ftell(f)) tempsize = ftell(f) - givenOffset;
				else tempsize *= 2;
				fseek( f, givenOffset, SEEK_SET );
				workbuffer = malloc( tempsize );
				fread( workbuffer, tempsize, 1, f );
				compressedsize = tempsize;
				resultbuffer = unpackBuffer( workbuffer, &resultsize, &compressedsize );
				if( !resultbuffer ) BREAK("error occured\n");
				tempsize = (bpp == image4bpp)?32:512;
				paletteData = malloc( tempsize );
				memcpy( paletteData, resultbuffer, tempsize );
				free(workbuffer);
				free(resultbuffer);
			}
			fseek( f, patchOffset+1, SEEK_SET );
			fread( &tempsize, 3, 1, f );
			fseek( f, 0, SEEK_END );
			if((patchOffset + 2*tempsize) > ftell(f)) tempsize = ftell(f) - patchOffset;
			else tempsize *= 2;
			fseek( f, patchOffset, SEEK_SET );
			workbuffer = malloc( tempsize );
			fread( workbuffer, tempsize, 1, f );
			compressedsize = tempsize;
			resultbuffer = unpackBuffer(workbuffer, &resultsize, &compressedsize);
			if( !resultbuffer ) BREAK("error occured\n");
			if(bpp == image8bpp) {
				if((tilesx*8*tilesy*8) != resultsize) BREAK("size/bpp dont match with uncompressed size\n");
			}
			else if(bpp == image4bpp) {
				if(((tilesx*8*tilesy*8)/2) != resultsize) BREAK("size/bpp dont match with uncompressed size\n");
			}
			rgbaPixelData = tiledImageWithPaletteToRGBA(resultbuffer, paletteData, tilesx, tilesy, bpp, 0);
			snprintf( outputname, 32, "%08d-patch-%08x.png", patchOffset, patchOffset );
			lodepng_encode32_file( outputname, rgbaPixelData, tilesx*8, tilesy*8 );
			free( rgbaPixelData );
			free(workbuffer);
			free(resultbuffer);
			free(paletteData);
			break;
		}
		case 3: { /* evidence */
			tilesx = 8;
			tilesy = 8;
			workbuffer = malloc(evidenceSize);
			for(i = 0; givenOffset < patchOffset; givenOffset += evidenceSize) {
				fseek( f, givenOffset, SEEK_SET );
				fread( workbuffer, evidenceSize, 1, f );
				rgbaPixelData = tiledImageToRGBA(workbuffer, tilesx, tilesy, image4bpp, 0);
				snprintf( outputname, 32, "evidence-%04d-%08x.png", i, givenOffset);
				lodepng_encode32_file( outputname, rgbaPixelData, tilesx*8, tilesy*8 );
				free(rgbaPixelData);
				i++;
			}
			free(workbuffer);
			break;
		}
		case 4: { /* compressed evidence */
			tilesx = 8;
			tilesy = 8;
			workbuffer = malloc(evidenceSize);
			for(i = 0; givenOffset < patchOffset; i++) {
				printf("given offset %08x\n", givenOffset);
				fseek( f, givenOffset, SEEK_SET );
				fread( workbuffer, evidenceSize, 1, f );
				compressedsize = evidenceSize;
				resultbuffer = unpackBuffer(workbuffer, &resultsize, &compressedsize);
				if(!resultbuffer) BREAK("error occured\n");
				if(resultsize != evidenceSize) {
					printf("%08x - ", givenOffset);
					BREAK("image is not evidence");
				}
				rgbaPixelData = tiledImageToRGBA(resultbuffer, tilesx, tilesy, image4bpp, 0);
				snprintf( outputname, 32, "cevidence-%04d-%08x.png", i, givenOffset);
				lodepng_encode32_file( outputname, rgbaPixelData, tilesx*8, tilesy*8 );
				free(rgbaPixelData);
				givenOffset += compressedsize;
				if(compressedsize % 4) givenOffset += (4-(compressedsize % 4));
			}
			free(workbuffer);
			break;
		}
		case 5: { /* raw image with palette */
			bpp = ((sourcetype==8)||(sourcetype==18))?image8bpp:image4bpp;
			fseek( f, patchOffset, SEEK_SET );
			if( (sourcetype - 10) > 0 ) transparency = 1;
			if( bpp == image8bpp ) {
				paletteData = malloc(512);
				fread( paletteData, 512, 1, f );
				fseek( f, givenOffset, SEEK_SET );
				workbuffer = malloc(tilesx*8*tilesy*8);
				fread( workbuffer, tilesx*8*tilesy*8, 1, f );
			}
			else {
				paletteData = malloc(32);
				fread( paletteData, 32, 1, f );
				fseek( f, givenOffset, SEEK_SET );
				workbuffer = malloc((tilesx*8*tilesy*8)/2);
				fread( workbuffer, (tilesx*8*tilesy*8)/2, 1, f );
			}
			rgbaPixelData = tiledImageWithPaletteToRGBA(workbuffer, paletteData, tilesx, tilesy, bpp, transparency);
			snprintf( outputname, 32, "raw-%08d-%08x.png", givenOffset, givenOffset);
			lodepng_encode32_file( outputname, rgbaPixelData, tilesx*8, tilesy*8 );
			free(rgbaPixelData);
			free(workbuffer);
			free(paletteData);
			break;
		}
		case 6: { /* AGB style striped image */
			fseek(f, givenOffset, SEEK_SET);
			fread(&agbpaloffset, 4, 1, f);
			numFiles = agbpaloffset/4;
			agboffsets = malloc(agbpaloffset-4);
			fread(agboffsets, agbpaloffset-4, 1, f);
			fseek(f, givenOffset+agbpaloffset+0x200, SEEK_SET); // seek to first data after possible 8bpp palette
			fread(minibuf, 4, 1, f);
			fseek(f, givenOffset+agbpaloffset, SEEK_SET);
			if(minibuf[0] == 0x10 && minibuf[1] == 0) { // bad assumptions about compressed files, but none appear to exceed 65k decompressed so yay?
				// image is 8bpp
				tempsize = 0x200;
				bpp = image8bpp;
				printf("detected 8bpp\n");
			}
			else {
				// image is 4bpp
				tempsize = 0x20;
				bpp = image4bpp;
				printf("detected 4bpp\n");
			}
			paletteData = malloc(tempsize);
			fread(paletteData, tempsize, 1, f);
			// now at first stripe
			compressedsize = tempsize = agboffsets[0]-agbpaloffset-tempsize;
			workbuffer = malloc(tempsize);
			fread(workbuffer, tempsize, 1, f);
			resultbuffer = unpackBuffer(workbuffer, &resultsize, &compressedsize);
			if( !resultbuffer ) BREAK("error occured first stripe\n");
			if(bpp == image8bpp) {
				if((tilesx*8*tilesy*8) != resultsize) BREAK("size/bpp dont match with uncompressed size\n");
			}
			else if(bpp == image4bpp) {
				if(((tilesx*8*tilesy*8)/2) != resultsize) BREAK("size/bpp dont match with uncompressed size\n");
			}
			fullbuffer = malloc(tilesx*8*(numFiles*tilesy)*8); // since list excludes stripe and includes palette, but number of entries matches up
			fulltarget = fullbuffer;
			memcpy(fulltarget, resultbuffer, resultsize);
			fulltarget += resultsize;
			free(workbuffer);
			free(resultbuffer);
			for(i = 0; i < numFiles-1; i++) {
				if(i == (numFiles-2)) tempsize = 0xFFFF; // just grab 64k of data for last stripe
				else tempsize = agboffsets[i+1] - agboffsets[i];
				
				compressedsize = tempsize;
				workbuffer = malloc(tempsize);
				fread(workbuffer, tempsize, 1, f);
				resultbuffer = unpackBuffer(workbuffer, &resultsize, &compressedsize);
				if( !resultbuffer ) BREAK("error occured\n");
				memcpy(fulltarget, resultbuffer, resultsize);
				fulltarget += resultsize;
				if(i == (numFiles-2)) tempsize = compressedsize; // remember compressed size of last stripe seperately
				free(workbuffer);
				free(resultbuffer);
			}
			rgbaPixelData = tiledImageWithPaletteToRGBA(fullbuffer, paletteData, tilesx, numFiles*tilesy, bpp, transparency);
			snprintf( outputname, 32, "%08d-striped-%08x.png", givenOffset, givenOffset );
			lodepng_encode32_file( outputname, rgbaPixelData, tilesx*8, numFiles*tilesy*8 );
			free( fullbuffer );
			free( rgbaPixelData );
			free( paletteData );
			printf("total size of striped image %08x\n", agboffsets[numFiles-2]+tempsize);
			
			free(agboffsets);
			
			break;
		}
		default: {
			BREAK("unknown type\n");
		}
	}
	printf("Done.\n");
	fclose(f);
	return 0;
	
}

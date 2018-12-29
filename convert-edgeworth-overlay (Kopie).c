#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

typedef struct {
	unsigned int add1;
	unsigned int add2;
	unsigned int add3;
}overlayPreset;

typedef struct {
	unsigned int set0;
	unsigned int set1;
	unsigned int set2;
	unsigned int set3;
	unsigned int set4;
	unsigned int set5;
}pointerSet;

overlayPreset presetList[9] = {
	{ 36, 128, 0 },		/*   0-143 */
	{ 40, 154, 32 },	/* 144-150 */
	{ 42, 34, 160 },	/* 151-191 */
	{ 42, 62, 128 },	/* 192-201 */
	{ 42, 216, 0 },		/* 202-206 */
	{ 45, 90, 96 },		/* 207-209 */
	{ 49, 47, 32 },		/* 210-212 */
	{ 50, 195, 32 },	/* 213-215 */
	{ 54, 132, 224 }	/* 216-220 */
};

char *speakers[] = {
	"empty - 00",		"???",			"Edgeworth",	"Gumshoe",		"Franziska - 04",
	"Teneiro",		"Meele",		"Lablanc",	"Passenger - 08",	"Passenger - 09",
	"Passenger - 10",	"PA Notice",		"Officer",	"Captain",		"Ernest",
	"Child",		"Passenger - 16",	"Customer",	"Vendor",		"Manella",
	"Manfred",		"Judge",		"Bailiff",	"Kay",			"Badd",
	"Yew",			"Faraday",		"Rell",		"Byrde",		"Portsman",
	"Forensics",		"Franziska - 31",	"Meekins",	"Oldbag",		"Lang",
	"Shih-na",		"Detective 1",		"Detective 2",	"Detective 3",		"Paups",
	"Ema",			"Lance",		"S.Samurai",	"P.Princess",		"Larry",
	"Alba",			"Palaeno",		"Reporter",	"Guard - 48",		"Guard - 49",
	"Missile",		"DeLite",		"Boy",		"Payne",		"Proto B.",
	"Faith",		"Defendant",		"Lawyer",	"Prosecutor",		"MIB",
	"Man - 60",		"Old Lady",		"Father",	"Daughter",		"Detective",
	"Blue Badger",		"Pink Badger",		"Man - 67",	"Woman",		"Cameraman",
	"Hart",			"Coachen",		"Chief",	"Fan",			"Friend"
};

/* fsck java for sorting things */
int compare (const void * a, const void * b) {
	//~ int x = *(int*)a;
	//~ int y = *(int*)b;
	//~ if(x<y) return -1;
	//~ else if(x>y) return 1;
	//~ return 0;
	return ( *(int*)a - *(int*)b );
}

char *charset[90] = {
	"~", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", 
	"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", 
	"L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", 
	"W", "X", "Y", "Z", 
	"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", 
	"l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", 
	"w", "x", "y", "z", 
	"!", "?", ".", "(", ")", ":", ",", "/", "*", "\'", 
	"+", "%", "&", "-", "'", "[", "]", "$", "#", ">", "<", 
	"=", "³", "é", "á", ";", " "
};

char *specialchars[8] = {
	"ä", "ö", "ü", "Ä", "Ö", "Ü", "ß", "€"
};

int main( int argc, char **argv ) {
	FILE *f, *o;
	unsigned int oversize, numpointer, preset, textidx = 0;
	unsigned int off0 = 2, off1, off2, off3;
	char textfile[0x10000] = {0}; /* 1M should be enough */
	
	/* transcribed things here */
	int i = 0, j = 0, k = 0, l = 0, m = 0, prog = 0, klasspoi = 0, folgen = 0, jumpout = 0;
	unsigned int buf = 0;
	uint32_t maxpointer[4];
	uint32_t pointerbuf[0x10000][6];
	uint32_t *pisort = NULL, *pointerpossort = NULL;
	pointerSet *pointer = NULL;
	unsigned char *overbufbyte = NULL, *evilbuf = NULL;
	uint32_t *overbufint = NULL;
	uint32_t hex1, hex2, hex3, hex4;
	unsigned char changer = 0;
	
	if( argc < 3 ) {
	printf("Not enough args!\nUse: %s [overlay file] [preset]\nwhere preset is\n0 -   0-143\n1 - 144-150\n2 - 151-191\n3 - 192-201\n4 - 202-206\n5 - 207-209\n6 - 210-212\n7 - 213-215\n8 - 216-220\n", argv[0]);
		return 1;
	}
	
	preset = strtoul(argv[2], NULL, 10);
	if( preset > 8 ) {
		printf("unsupported preset %d\n", preset+1);
		return 1;
	}
	
	off1 = presetList[preset].add1;
	off2 = presetList[preset].add2;
	off3 = presetList[preset].add3;
	
	if( !(f = fopen( argv[1], "rb" ))) {
		printf("Couldnt open file %s\n", argv[1]);
		return 1;
	}
	
	fseek(f, 0, SEEK_END);
	oversize = ftell(f);
	fseek(f, 0, SEEK_SET);
	
	printf("oversize %08x\n", oversize);
	
	char *outfilename = malloc( strlen( argv[1] ) + 4 + 1 );
	
	sprintf( outfilename, "%s.txt", argv[1] );
	
	/* beginning bullshit here, see line 45 in cut */
	
	overbufbyte = malloc(oversize);
	overbufint = malloc(oversize*sizeof(uint32_t));
	evilbuf = (unsigned char *) overbufint;
	
	fread(overbufbyte, oversize, 1, f);
	fclose(f);
	hex1 = (oversize & 0xFF000000) >> 24;
	hex2 = (oversize & 0xFF0000) >> 16;
	hex3 = (oversize & 0xFF00) >> 8;
	hex4 = oversize & 0xFF;
	printf("%04x %04x %04x %04x\n", hex1, hex2, hex3, hex4);
	hex4 += off3;
	hex3 += (off2+(hex4>>8));
	hex2 += (off1+(hex3>>8));
	hex1 += (off0+(hex2>>8));
	printf("%04x %04x %04x %04x\n", hex1, hex2, hex3, hex4);
	hex4 &= 0xFF;
	hex3 &= 0xFF;
	hex2 &= 0xFF;
	hex1 &= 0xFF;
	printf("%04x %04x %04x %04x\n", hex1, hex2, hex3, hex4);
	maxpointer[0] = hex1;
	maxpointer[1] = hex2;
	maxpointer[2] = hex3;
	maxpointer[3] = hex4;
	for(i=0;i<oversize;i++) evilbuf[i] = overbufbyte[i] & 0xFF;
	
	/* stupid switch starts here, see line 76 in cut */
	for(i = oversize-1; i > 3; i--) {
		if(evilbuf[i] == off0) {
			if(evilbuf[i-1] == off1) {
				if((evilbuf[i-2] == off2) && (evilbuf[i-3] > off3)) {
					pointerbuf[k][0] = evilbuf[i];
					pointerbuf[k][1] = evilbuf[i-1];
					pointerbuf[k][2] = evilbuf[i-2];
					pointerbuf[k][3] = evilbuf[i-3];
					pointerbuf[k][4] = i-3;
					k++;
				}
				if(evilbuf[i-2] > off2) {
					pointerbuf[k][0] = evilbuf[i];
					pointerbuf[k][1] = evilbuf[i-1];
					pointerbuf[k][2] = evilbuf[i-2];
					pointerbuf[k][3] = evilbuf[i-3];
					pointerbuf[k][4] = i-3;
					k++;
				}
			}
			else if(evilbuf[i-1] == maxpointer[1]) {
				if((evilbuf[i-2] == maxpointer[2]) && (evilbuf[i-3] < maxpointer[3])) {
					pointerbuf[k][0] = evilbuf[i];
					pointerbuf[k][1] = evilbuf[i-1];
					pointerbuf[k][2] = evilbuf[i-2];
					pointerbuf[k][3] = evilbuf[i-3];
					pointerbuf[k][4] = i-3;
					k++;
				}
				if(evilbuf[i-2] < maxpointer[2]) {
					pointerbuf[k][0] = evilbuf[i];
					pointerbuf[k][1] = evilbuf[i-1];
					pointerbuf[k][2] = evilbuf[i-2];
					pointerbuf[k][3] = evilbuf[i-3];
					pointerbuf[k][4] = i-3;
					k++;
				}
			}
			else if((evilbuf[i-1] < maxpointer[1]) && (evilbuf[i-1] > off1)) {
				pointerbuf[k][0] = evilbuf[i];
				pointerbuf[k][1] = evilbuf[i-1];
				pointerbuf[k][2] = evilbuf[i-2];
				pointerbuf[k][3] = evilbuf[i-3];
				pointerbuf[k][4] = i-3;
				k++;
			}
		}
	}
	j = k;
	numpointer = k;
	pointer = malloc(sizeof(pointerSet)*numpointer);
	memset(pointer, 0, sizeof(pointerSet)*numpointer);
	
	buf = off0 << 8;
	buf = (buf + off1) << 8;
	buf = (buf + off2) << 8;
	buf += off3;
	
	for(i = j-1; i >= 0; i--) {
		pointer[l].set0 = pointerbuf[i][0] << 8;
		pointer[l].set0 = (pointer[l].set0 + pointerbuf[i][1]) << 8;
		pointer[l].set0 = (pointer[l].set0 + pointerbuf[i][2]) << 8;
		pointer[l].set0 += pointerbuf[i][3];
		pointer[l].set0 -= buf;
		pointer[l].set2 = pointerbuf[i][4];
		pointer[l].set3 = pointerbuf[i][5];
		++l;
	}
	for(i = 0; i < 0x10000; i++) {
		pointerbuf[i][0] = 0;
		pointerbuf[i][1] = 0;
		pointerbuf[i][2] = 0;
		pointerbuf[i][3] = 0;
		pointerbuf[i][4] = 0;
		pointerbuf[i][5] = 0;
	}
	for(i = 0; i < k; i++) {
		pointerbuf[i][0] = pointer[i].set0;
		pointerbuf[i][2] = pointer[i].set2;
		pointerbuf[i][3] = pointer[i].set3;
	}
	pisort = malloc(sizeof(uint32_t)*numpointer);
	pointerpossort = malloc(sizeof(uint32_t)*numpointer);
	for(i = 0; i < numpointer; i++) {
		pisort[i] = pointer[i].set0;
		pointerpossort[i] = pointer[i].set2;
	}
	
	qsort(pisort, numpointer, sizeof(uint32_t), compare);
	qsort(pointerpossort, numpointer, sizeof(uint32_t), compare);
	for(j = 0; j < numpointer; j++) {
		for(i = 0; i < numpointer; i++) {
			if(pisort[j] == pointerbuf[i][0]) {
				pointer[j].set0 = pointerbuf[i][0];
				pointerbuf[i][0] = 0;
				pointer[j].set2 = pointerbuf[i][2];
				pointer[j].set3 = pointerbuf[i][3];
				i = numpointer;
			}
		}
	}
	
	k = 0; l = 1; m = 1;
	if(numpointer/100 > 0) l = numpointer/100;
	if(numpointer%100/10 >=5) l++;
	prog = l;
	for(i = 0; i < numpointer; i++) {
		for(j = pointer[i].set0; j < oversize-2; j++) {
			if((j > pointerpossort[k]) && (k < numpointer-1)) k++;
			if((evilbuf[j] == 255) && ((evilbuf[j+1] == 3) || (evilbuf[j+1] == 4)) && (evilbuf[j+2] == 0)) {
				pointer[i].set1 = j+2;
				pointer[i].set3 = m;
				m++;
				pointer[i].set4 = 1;
				if(i > 1) {
					for(klasspoi = i-1; klasspoi >=0; klasspoi--) {
						if((pointer[klasspoi].set4 == 1) && (pointer[klasspoi].set1 == pointer[i].set1)) {
							pointer[i].set3 = 0;
							pointer[i].set5 = pointer[klasspoi].set3;
							pointer[i].set4 = 3;
							m--;
							klasspoi = -1;
						}
					}
				}
				j = oversize;
				if(l == 0) l = prog;
				l--;
			}
			else if((j == pointerpossort[k]) && (evilbuf[j-1] != 255)) {
				pointer[i].set1 = j+3;
				pointer[i].set4 = 2;
				j = oversize;
				if(l == 0) l = prog;
				l--;
			}
		}
	}
	
	for(i = 0; i < numpointer; i++) {
		pointerbuf[i][0] = pointer[i].set0;
		pointerbuf[i][1] = pointer[i].set1;
		pointerbuf[i][2] = pointer[i].set2;
		pointerbuf[i][3] = pointer[i].set3;
		pointerbuf[i][4] = pointer[i].set4;
		pointerbuf[i][5] = pointer[i].set5;
	}
	for(i = 0; i < numpointer; i++) {
		for(j = 0; j < numpointer; j++) {
			if(pointerpossort[i] == pointerbuf[j][2]) {
				pointer[i].set0 = pointerbuf[j][0];
				pointer[i].set1 = pointerbuf[j][1];
				pointer[i].set2 = pointerbuf[j][2];
				pointerbuf[j][2] = 0; /* wat */
				pointer[i].set3 = pointerbuf[j][3];
				pointer[i].set4 = pointerbuf[j][4];
				pointer[i].set5 = pointerbuf[j][5];
				break;
			}
		}
	}
	
	folgen = -1;
	k = 0;
	for(l = 0; l < numpointer; l++) {
		if(pointer[l].set4 == 1) {
			pointer[l].set5 = k;
			k++;
			printf("set0 %d set1 %d\n", pointer[l].set0, pointer[l].set1);
			for(i = pointer[l].set0; i <= pointer[l].set1; i++) {
				changer = evilbuf[i] & 0xFF;
				if(folgen == 0) {
					strcat(textfile, "]");
					textidx++;
					folgen = -1;
				}
				//~ if(folgen > 0) folgen--;
				if(folgen > 0) {
					strcat(textfile, "(");
					textidx++;
					while(folgen > 1) {
						sprintf(textfile+textidx, "%02x, ", evilbuf[i]);
						i++;
						textidx+=4;
						folgen--;
					}
					sprintf(textfile+textidx, "%02x)", evilbuf[i]);
					//~ i++;
					folgen--;
					textidx+=3;
					continue;
				}
				if((changer >= 0xFE) && (folgen < 0)) {
				//~ printf("changer is %x\n", changer);
					if(changer == 0xFE) {
						strcat(textfile, "\n");
						textidx+=1;
					}
					if((changer == 0xFF) && (i < oversize-1)) {
				//~ printf("interest is %x\n", evilbuf[i+1]);
						switch((char)evilbuf[i+1]) {
							case 0x03: {
								strcat(textfile, "[NewPage?]");
								textidx+=10;
								i+=2;
								break;
							}
							case 0x04: {
								strcat(textfile, "[NewPage]");
								textidx+=9;
								i+=2;
								break;
							}
							case 0x05: {
								strcat(textfile, "[Pause|");
								textidx+=7;
								folgen=1;
								i++;
								break;
							}
							case 0x07: {
								strcat(textfile, "[->|");
								textidx+=4;
								folgen=1;
								i++;
								break;
							}
							case 0x0B: {
								strcat(textfile, "[Sound|");
								textidx+=7;
								folgen=1;
								i++;
								break;
							}
							case 0x09: {
								strcat(textfile, "[~~|");
								textidx+=4;
								folgen=2;
								i++;
								break;
							}
							case 0x08: {
								strcat(textfile, "[Flash|");
								textidx+=7;
								folgen=2;
								i++;
								break;
							}
							case 0x02: {
								strcat(textfile, "[Color|");
								textidx+=7;
								folgen=1;
								i++;
								break;
							}
							case 0x0E: {
								strcat(textfile, "[Ani|");
								textidx+=5;
								folgen=4;
								i++;
								break;
							}
							case 0x0A: {
								strcat(textfile, "[Music|");
								textidx+=7;
								folgen=1;
								i++;
								break;
							}
							case 0x15: {
								strcat(textfile, "[MFadeout|");
								textidx+=10;
								folgen=1;
								i++;
								break;
							}
							case 0x16: {
								strcat(textfile, "[Speed|");
								textidx+=7;
								folgen=1;
								i++;
								break;
							}
							case 0x1B: {
								strcat(textfile, "[STR|Q]");
								textidx+=7;
								folgen=-1;
								i++;
								break;
							}
							case 0x18: {
								strcat(textfile, "[Pic|");
								textidx+=5;
								folgen=3;
								i++;
								break;
							}
							case 0x10: {
								strcat(textfile, "[Portrait|");
								textidx+=10;
								folgen=3;
								i++;
								break;
							}
							case 0x1F: {
								strcat(textfile, "[/Pkt|");
								textidx+=6;
								folgen=1;
								i++;
								break;
							}
							case 0x20: {
								strcat(textfile, "[Pkt|");
								textidx+=5;
								folgen=1;
								i++;
								break;
							}
							case 0x17: {
								strcat(textfile, "[Graphic|");
								textidx+=9;
								folgen=4;
								i++;
								break;
							}
							case 0x1D: {
								strcat(textfile, "[STR:S|");
								textidx+=7;
								folgen=1;
								i++;
								break;
							}
							case 0x21: {
								strcat(textfile, "[black|");
								textidx+=7;
								folgen=2;
								i++;
								break;
							}
							case 0x22: {
								strcat(textfile, "[/black|");
								textidx+=8;
								folgen=2;
								i++;
								break;
							}
							case 0x11: {
								strcat(textfile, "[BG|");
								textidx+=4;
								folgen=14;
								i++;
								break;
							}
							case 0x14: {
								strcat(textfile, "[STR:J|");
								textidx+=7;
								folgen=1;
								i++;
								break;
							}
							case 0x26: {
								strcat(textfile, "[Sprite|");
								textidx+=8;
								folgen=1;
								i++;
								break;
							}
							default: {
								// sprintf(textfile+textidx, "{0x%2x}", (unsigned int)buffer[i+1]);
								strcat(textfile, "{0xFF}");
								textidx+=6;
								break;
							}
						}
					}
				}
				else {
					j = 1;
					while(j<90 && !jumpout) {
						if(changer==(char)j) {
							changer = *charset[j];
							jumpout=1;
						}
						j++;
					}
					j=0;
					while(j<8 && !jumpout) {
						if(changer==(char)j+115) {
							changer = *specialchars[j];
							jumpout=1;
						}
						j++;
					}
					if(!jumpout) {
						sprintf(textfile+textidx, "{0x%02x}", changer);
						textidx+=6;
					}
					else {
						jumpout=0;
						sprintf(textfile+textidx, "%c", changer);
						textidx++;
					}
				}
				
			}
			strcat(textfile, "\n");
			textidx++;
		}
	}
	
	if(textidx) {
		if( !(o = fopen( outfilename, "w" ))) {
				printf("Couldnt open file %s\n", outfilename);
				return 1;
		}
		fwrite(textfile, textidx, 1, o);
		fclose(o);
	}
	else printf("No text in overlay!\n");
	
	free(outfilename);
	free(overbufbyte);
	free(overbufint);
	free(pointer);
	free(pisort);
	free(pointerpossort);
	
	
	printf("Done.\n");
	return 0;
}

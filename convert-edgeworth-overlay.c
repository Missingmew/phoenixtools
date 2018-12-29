#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

uint32_t presets[9] = {
	0x02248000,
	0x02289A20,
	0x022A22A0,
	0x022A3E80,
	0x022AD800,
	0x022D5A60,
	0x02312F20,
	0x0232C320,
	0x023684E0
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

int compare (const void * a, const void * b) {
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
	unsigned int oversize, preset, textidx = 0, textsize = 0x10000;
	char *textfile = NULL;
	
	/* transcribed things here */
	int folgen = 0, jumpout = 0;
	unsigned int i, j, k;
	unsigned char *overbufbyte = NULL;
	unsigned char changer = 0;
	
	unsigned int numpointers, endofpage;
	uint32_t newpointerbuf[0x1000];
	uint32_t newmaxpointer;
	uint32_t firstpointeroffset = 0;
	
	if( argc < 3 ) {
	printf("Not enough args!\nUse: %s [overlay file] [preset]\nwhere preset is\n0 -   0-143\n1 - 144-150\n2 - 151-191\n3 - 192-201\n4 - 202-206\n5 - 207-209\n6 - 210-212\n7 - 213-215\n8 - 216-220\n", argv[0]);
		return 1;
	}
	
	preset = strtoul(argv[2], NULL, 10);
	if( preset > 8 ) {
		printf("unsupported preset %d\n", preset+1);
		return 1;
	}
	
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
	fread(overbufbyte, oversize, 1, f);
	
	textfile = malloc(textsize);
	
	//
	newmaxpointer = oversize + presets[preset];
	
	for(i = 0, numpointers = 0; i < oversize-4; i++) {
		uint32_t tocheck = *(uint32_t *)(overbufbyte+i);
		if(tocheck < newmaxpointer && tocheck >= presets[preset]) {
			if(!firstpointeroffset) firstpointeroffset = i;
			if(i%4) printf("wat numpointer %d\n", numpointers);
			newpointerbuf[numpointers] = tocheck - presets[preset];
			numpointers++;
		}
	}
	
	for(i = 0; i < numpointers; i++) {
		//~ if(pointer[i].set0 != newpointerbuf[i]) printf("pointer %d differs %08x vs %08x ", i, pointer[i].set0, newpointerbuf[i]);
		printf("newpointerbuf points to %08x, word there is %08x\n", newpointerbuf[i], *(uint32_t *)(overbufbyte+newpointerbuf[i]));
		//~ if(pointer[i].set2 != newpointeroffsets[i]*4) printf("index %d differs %08x vs %08x(%08x)", i, pointer[i].set2, newpointeroffsets[i]*4, newpointeroffsets[i]);
	}
	
	//~ for(i = 0; i < numptypes; i++) printf("ptype %03d points to %08x with type %d\n", i, ptypes[i].pointer, ptypes[i].type);
	
	// DEBUG
	//~ return 0;
	
	folgen = -1;
	k = 0;
	for(i = 0; i < numpointers; i++) {
		if(textidx > textsize/2) {
			textsize *= 2;
			textfile = realloc(textfile, textsize);
		}
		printf("i %d numpointers %d\n", i, numpointers);
		uint32_t targetword = *(uint32_t *)(overbufbyte+newpointerbuf[i]);
		if((targetword & 0x00FFFFFF) < 0x100) {
			printf("skipping potentially useless pointer\n");
			continue;
		}
		endofpage = 0;
		for(j = newpointerbuf[i]; !endofpage && (j < oversize); j++) {
			//~ printf("j %08x addr %p limit %p to %p\n", j, overbufbyte+j, overbufbyte, overbufbyte+oversize);
			fflush(stdout);
			changer = overbufbyte[j];
			if(folgen == 0) {
				sprintf(textfile+textidx, "]");
				textidx++;
				folgen = -1;
			}
			//~ if(folgen > 0) folgen--;
			if(folgen > 0) {
				sprintf(textfile+textidx, "(");
				textidx++;
				while(folgen > 1) {
					sprintf(textfile+textidx, "%02x, ", overbufbyte[j]);
					j++;
					textidx+=4;
					folgen--;
				}
				sprintf(textfile+textidx, "%02x)", overbufbyte[j]);
				//~ j++;
				folgen--;
				textidx+=3;
				continue;
			}
			if((changer >= 0xFE) && (folgen < 0)) {
				//~ printf("changer is %x\n", changer);
				if(changer == 0xFE) {
					sprintf(textfile+textidx, "\n");
					textidx+=1;
				}
				if((changer == 0xFF) && (j < oversize-1)) {
					//~ printf("interest is %x\n", overbufbyte[j+1]);
					switch(overbufbyte[j+1]) {
						case 0x03: {
							sprintf(textfile+textidx, "[NewPage?]\n");
							textidx+=11;
							j+=2;
							break;
						}
						case 0x04: {
							sprintf(textfile+textidx, "[NewPage]\n");
							textidx+=10;
							j+=2;
							endofpage = 1;
							break;
						}
						case 0x05: {
							sprintf(textfile+textidx, "[Pause|");
							textidx+=7;
							folgen=1;
							j++;
							break;
						}
						case 0x07: {
							sprintf(textfile+textidx, "[->|");
							textidx+=4;
							folgen=1;
							j++;
							break;
						}
						case 0x0B: {
							sprintf(textfile+textidx, "[Sound|");
							textidx+=7;
							folgen=1;
							j++;
							break;
						}
						case 0x09: {
							sprintf(textfile+textidx, "[DTilde|");
							textidx+=8;
							folgen=2;
							j++;
							break;
						}
						case 0x08: {
							sprintf(textfile+textidx, "[Flash|");
							textidx+=7;
							folgen=2;
							j++;
							break;
						}
						case 0x02: {
							sprintf(textfile+textidx, "[Color|");
							textidx+=7;
							folgen=1;
							j++;
							break;
						}
						case 0x0E: {
							sprintf(textfile+textidx, "[Ani|");
							textidx+=5;
							folgen=4;
							j++;
							break;
						}
						case 0x0A: {
							sprintf(textfile+textidx, "[Music|");
							textidx+=7;
							folgen=1;
							j++;
							break;
						}
						case 0x15: {
							sprintf(textfile+textidx, "[MFadeout|");
							textidx+=10;
							folgen=1;
							j++;
							break;
						}
						case 0x16: {
							sprintf(textfile+textidx, "[Speed|");
							textidx+=7;
							folgen=1;
							j++;
							break;
						}
						case 0x1B: {
							sprintf(textfile+textidx, "[STR|Q]");
							textidx+=7;
							folgen=-1;
							j++;
							break;
						}
						case 0x18: {
							sprintf(textfile+textidx, "[Pic|");
							textidx+=5;
							folgen=3;
							j++;
							break;
						}
						case 0x10: {
							sprintf(textfile+textidx, "[Portrait|");
							textidx+=10;
							folgen=3;
							j++;
							break;
						}
						case 0x1F: {
							sprintf(textfile+textidx, "[/Pkt|");
							textidx+=6;
							folgen=1;
							j++;
							break;
						}
						case 0x20: {
							sprintf(textfile+textidx, "[Pkt|");
							textidx+=5;
							folgen=1;
							j++;
							break;
						}
						case 0x17: {
							sprintf(textfile+textidx, "[Graphic|");
							textidx+=9;
							folgen=4;
							j++;
							break;
						}
						case 0x1D: {
							sprintf(textfile+textidx, "[STR:S|");
							textidx+=7;
							folgen=1;
							j++;
							break;
						}
						case 0x21: {
							sprintf(textfile+textidx, "[black|");
							textidx+=7;
							folgen=2;
							j++;
							break;
						}
						case 0x22: {
							sprintf(textfile+textidx, "[/black|");
							textidx+=8;
							folgen=2;
							j++;
							break;
						}
						case 0x11: {
							sprintf(textfile+textidx, "[BG|");
							textidx+=4;
							folgen=14;
							j++;
							break;
						}
						case 0x14: {
							sprintf(textfile+textidx, "[STR:J|");
							textidx+=7;
							folgen=1;
							j++;
							break;
						}
						case 0x26: {
							sprintf(textfile+textidx, "[Sprite|");
							textidx+=8;
							folgen=1;
							j++;
							break;
						}
						default: {
							sprintf(textfile+textidx, "{0x%2x}", overbufbyte[j+1]);
							textidx+=6;
							break;
						}
					}
				}
			}
			else {
				k = 1;
				while(k<90 && !jumpout) {
					if(changer==k) {
						changer = *charset[k];
						jumpout=1;
					}
					k++;
				}
				k=0;
				while(k<8 && !jumpout) {
					if(changer==k+115) {
						changer = *specialchars[k];
						jumpout=1;
					}
					k++;
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
		sprintf(textfile+textidx, "\n");
		textidx++;
	}
	sprintf(textfile+textidx, "\n");
	textidx++;
	
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
	free(textfile);
	
	fclose(f);
	
	printf("Done.\n");
	return 0;
}

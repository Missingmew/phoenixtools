#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data_support.h"
#include "param.h"
#include "common.h"

// apollo has 60...
#define MAXPERSONS 64
#define REALLOCNUM 10

struct gba_personanim {
	unsigned offset;
	char *name;
};

struct datalist_gba_person {
	unsigned numsizes;
	unsigned *sizes;
	struct gba_personanim **anims;
};

struct datalist {
	unsigned size;
	char **names;
};

struct datalist sounds;
struct datalist speakers;
struct datalist persons;
struct datalist bgs;
struct datalist locations;
struct datalist evidences;
struct datalist animations;

struct datalist_gba_person gba_persons;

char *data_getname_regular(enum datatypes type, unsigned id) {
	switch(type) {
		case DATA_SOUND: {
			if(id < sounds.size) return sounds.names[id];
			break;
		}
		case DATA_SPEAKER: {
			if(id < speakers.size) return speakers.names[id];
			break;
		}
		case DATA_PERSONNDS: {
			if(id < persons.size) return persons.names[id];
			break;
		}
		case DATA_BACKGROUND: {
			if(id < bgs.size) return bgs.names[id];
			break;
		}
		case DATA_LOCATION: {
			if(id < locations.size) return locations.names[id];
			break;
		}
		case DATA_EVIDENCE: {
			if(id < evidences.size) return evidences.names[id];
			break;
		}
		case DATA_ANIMATION: {
			if(id < animations.size) return animations.names[id];
			break;
		}
		default: break;
	}
	return NULL;
}

char *data_getname_gba_person(unsigned person, unsigned offset) {
	if(person < gba_persons.numsizes) {
		for(unsigned i = 0; i < gba_persons.sizes[person]; i++) {
			if(gba_persons.anims[person][i].offset == offset) return gba_persons.anims[person][i].name;
		}
	}
	return NULL;
}

char *data_getname(enum datatypes type, unsigned id, unsigned offset) {
	switch(type) {
		case DATA_SOUND:
		case DATA_SPEAKER:
		case DATA_PERSONNDS:
		case DATA_BACKGROUND:
		case DATA_LOCATION:
		case DATA_EVIDENCE:
		case DATA_ANIMATION: {
			return data_getname_regular(type, id);
		}
		case DATA_PERSONGBA: {
			return data_getname_gba_person(id, offset);
		}
		default: {
			fprintf(stderr, "tried data_getname with unknown type %d and id %u and offset %x!\n", type, id, offset);
			break;
		}
	}
	return NULL;
}

int data_getindex_regular(enum datatypes type, char *str) {
	switch(type) {
		case DATA_SOUND: {
			for(unsigned i = 0; i < sounds.size; i++) {
				if(sounds.names[i] && !strcmp(str, sounds.names[i])) return i;
			}
			break;
		}
		case DATA_SPEAKER: {
			for(unsigned i = 0; i < speakers.size; i++) {
				if(speakers.names[i] && !strcmp(str, speakers.names[i])) return i;
			}
			break;
		}
		case DATA_PERSONNDS: {
			for(unsigned i = 0; i < animations.size; i++) {
				if(animations.names[i] && !strcmp(str, animations.names[i])) return i;
			}
			break;
		}
		case DATA_BACKGROUND: {
			for(unsigned i = 0; i < bgs.size; i++) {
				if(bgs.names[i] && !strcmp(str, bgs.names[i])) return i;
			}
			break;
		}
		case DATA_LOCATION: {
			for(unsigned i = 0; i < locations.size; i++) {
				if(locations.names[i] && !strcmp(str, locations.names[i])) return i;
			}
			break;
		}
		case DATA_EVIDENCE: {
			for(unsigned i = 0; i < evidences.size; i++) {
				if(evidences.names[i] && !strcmp(str, evidences.names[i])) return i;
			}
			break;
		}
		case DATA_ANIMATION: {
			for(unsigned i = 0; i < animations.size; i++) {
				if(animations.names[i] && !strcmp(str, animations.names[i])) return i;
			}
			break;
		}
		default: break;
	}
	return -1;
}

int data_getoffset_gba_person(char *str, unsigned person) {
	if(person < gba_persons.numsizes) {
		for(unsigned i = 0; i < gba_persons.sizes[person]; i++) {
			if(gba_persons.anims[person][i].name && !strcmp(str, gba_persons.anims[person][i].name)) return gba_persons.anims[person][i].offset;
		}
	}
	return -1;
}

int data_getindexoffset(enum datatypes type, char *str, unsigned person) {
	switch(type) {
		case DATA_SOUND:
		case DATA_SPEAKER:
		case DATA_PERSONNDS:
		case DATA_BACKGROUND:
		case DATA_LOCATION:
		case DATA_EVIDENCE:
		case DATA_ANIMATION: {
			return data_getindex_regular(type, str);
		}
		case DATA_PERSONGBA: {
			return data_getoffset_gba_person(str, person);
		}
		default: {
			fprintf(stderr, "tried data_getindex with unknown type %d and str %s and person %u!\n", type, str, person);
			break;
		}
	}
	return -1;
}

void data_loadfile_regular(FILE *f, struct datalist *dat) {
	unsigned curalloc = 0;
	int scanindex, maxindex = -1;
	unsigned line = 0;
	char scanline[512];
	char scanname[256];
	unsigned scannamestart, scannameend;
	int numitems;
	char **newnamearr = NULL;
	
	if(!f) return;
	
	while(!feof(f)) {
		line++;
		if(!fgets(scanline, 512, f)) scanline[0] = 0;
		numitems = sscanf(scanline, "%d = %n%255s%n\n", &scanindex, &scannamestart, scanname, &scannameend);
		if(numitems == 2) {
			unsigned scannamelen = scannameend-scannamestart;
			//~ printf("line %u: found index %u for name %s %u-%u\n", line, scanindex, scanname, scannamestart, scannameend);
			maxindex = (scanindex > maxindex) ? scanindex : maxindex;
			while(scanindex >= curalloc) {
				curalloc += REALLOCNUM;
				newnamearr = realloc(newnamearr, curalloc*sizeof(char *));
				// make sure that new elements are null
				memset(newnamearr+curalloc-REALLOCNUM, 0, sizeof(char *)*REALLOCNUM);
			}
			
			newnamearr[scanindex] = malloc(scannamelen+1);
			memcpy(newnamearr[scanindex], scanname, scannamelen+1);
		}
	}
	
	dat->size = maxindex+1;
	dat->names = newnamearr;
}

void data_loadfile_gba_person(FILE *f, struct datalist_gba_person *dat) {
	unsigned scanoffset, curalloc = 0;
	int scanindex, maxindex = -1;
	unsigned *curallocs = NULL;
	unsigned line = 0;
	char scanline[512];
	char scanname[256];
	unsigned scannamestart, scannameend;
	int numitems;
	
	unsigned *newsizearr = NULL;
	struct gba_personanim **newanimarr = NULL;
	
	if(!f) return;
	
	while(!feof(f)) {
		line++;
		fgets(scanline, 512, f);
		numitems = sscanf(scanline, "%d, %x = %n%255s%n\n", &scanindex, &scanoffset, &scannamestart, scanname, &scannameend);
		if(numitems == 3) {
			unsigned scannamelen = scannameend-scannamestart;
			//~ printf("line %u: found index %u and offset %x for name %s\n", line, scanindex, scanoffset, scanname);
			if(scanindex >= MAXPERSONS) {
				printf("scanned index exceeds MAXPERSONS limit of %u, ignoring this line!\n", MAXPERSONS);
				continue;
			}
			maxindex = (scanindex > maxindex) ? scanindex : maxindex;
			// if we found a person id greater than what we have alloced for, expand allocated arrays
			while(scanindex >= curalloc) {
				curalloc += REALLOCNUM;
				
				// realloc arrays that hold current allocated space and elements for each person
				curallocs = realloc(curallocs, curalloc*sizeof(unsigned));
				newsizearr = realloc(newsizearr, curalloc*sizeof(unsigned));
				newanimarr = realloc(newanimarr, curalloc*sizeof(struct gba_personanim *));
				
				// make sure that new elements are null
				memset(curallocs+curalloc-REALLOCNUM, 0, sizeof(unsigned)*REALLOCNUM);
				memset(newsizearr+curalloc-REALLOCNUM, 0, sizeof(unsigned)*REALLOCNUM);
				memset(newanimarr+curalloc-REALLOCNUM, 0, sizeof(struct gba_personanim *)*REALLOCNUM);
			}
			// next, check if the subarray has enough space, if not expand
			if(newsizearr[scanindex] == curallocs[scanindex]) {
				curallocs[scanindex] += REALLOCNUM;
				newanimarr[scanindex] = realloc(newanimarr[scanindex], curallocs[scanindex]*sizeof(struct gba_personanim));
				// make sure that new elements are null
				memset(newanimarr[scanindex]+curallocs[scanindex]-REALLOCNUM, 0, sizeof(struct gba_personanim)*REALLOCNUM);
			}
			// finally, allocate space for the name and copy over scanned data
			
			newanimarr[scanindex][newsizearr[scanindex]].name = malloc(scannamelen+1);
			memcpy(newanimarr[scanindex][newsizearr[scanindex]].name, scanname, scannamelen+1);
			newanimarr[scanindex][newsizearr[scanindex]++].offset = scanoffset;
		}
	}
	
	dat->numsizes = maxindex+1;
	dat->sizes = newsizearr;
	dat->anims = newanimarr;
	
	free(curallocs);
}

void data_loadfile(enum datatypes type, char *file) {
	FILE *f;
	
	if(!file) return;
	
	if(!(f = fopen(file, "rb"))) {
		fprintf(stderr, "could not open %s for reading\n", file);
		return;
	}
	
	switch(type) {
		case DATA_SOUND: {
			data_loadfile_regular(f, &sounds);
			break;
		}
		case DATA_SPEAKER: {
			data_loadfile_regular(f, &speakers);
			break;
		}
		case DATA_PERSONNDS: {
			data_loadfile_regular(f, &persons);
			break;
		}
		case DATA_BACKGROUND: {
			data_loadfile_regular(f, &bgs);
			break;
		}
		case DATA_LOCATION: {
			data_loadfile_regular(f, &locations);
			break;
		}
		case DATA_EVIDENCE: {
			data_loadfile_regular(f, &evidences);
			break;
		}
		case DATA_ANIMATION: {
			data_loadfile_regular(f, &animations);
			break;
		}
		case DATA_PERSONGBA: {
			data_loadfile_gba_person(f, &gba_persons);
			break;
		}
		default: {
			fprintf(stderr, "tried to data_loadfile with unknown type %d and file %s!\n", type, file);
			break;
		}
	}
	
	fclose(f);
}

void data_loadfilesfromparams(struct params *param) {
	data_loadfile(DATA_SOUND, param->soundfile);
	data_loadfile(DATA_SPEAKER, param->speakerfile);
	if(ISNDS(param->gamenum)) data_loadfile(DATA_PERSONNDS, param->persfile);
	else data_loadfile(DATA_PERSONGBA, param->persfile);
	data_loadfile(DATA_BACKGROUND, param->bgfile);
	data_loadfile(DATA_LOCATION, param->locationfile);
	data_loadfile(DATA_EVIDENCE, param->evidencefile);
	data_loadfile(DATA_ANIMATION, param->animfile);
}

void data_cleanup_generic(struct datalist *dat) {
	for(unsigned i = 0; i < dat->size; i++) {
		free(dat->names[i]);
	}
	free(dat->names);
}

void data_cleanup_gba_person(struct datalist_gba_person *dat) {
	for(unsigned i = 0; i < dat->numsizes; i++) {
		for(unsigned j = 0; j < dat->sizes[i]; j++) {
			free(dat->anims[i][j].name);
		}
		free(dat->anims[i]);
	}
	free(dat->sizes);
	free(dat->anims);
}

void data_cleanup(void) {
	data_cleanup_generic(&sounds);
	data_cleanup_generic(&speakers);
	data_cleanup_generic(&persons);
	data_cleanup_generic(&bgs);
	data_cleanup_generic(&locations);
	data_cleanup_generic(&evidences);
	data_cleanup_generic(&animations);
	data_cleanup_gba_person(&gba_persons);
}

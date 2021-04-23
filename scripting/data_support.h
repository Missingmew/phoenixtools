#ifndef DATA_SUPPORT_H_
#define DATA_SUPPORT_H_

enum datatypes {
	DATA_SOUND,
	DATA_SPEAKER,
	DATA_ANIMATIONNDS,
	DATA_ANIMATIONGBA,
	DATA_BACKGROUND,
	DATA_LOCATION
};

char *data_getname(enum datatypes type, unsigned id, unsigned offset);

int data_getindexoffset(enum datatypes type, char *str, unsigned person);

void data_loadfile(enum datatypes type, char *file);

void data_cleanup(void);

#endif

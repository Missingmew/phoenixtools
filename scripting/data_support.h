#ifndef DATA_SUPPORT_H_
#define DATA_SUPPORT_H_

#include "param.h"

enum datatypes {
	DATA_SOUND,
	DATA_SPEAKER,
	DATA_ANIMATIONNDS,
	DATA_ANIMATIONGBA,
	DATA_BACKGROUND,
	DATA_LOCATION,
	DATA_EVIDENCE,
	DATA_PROFILE
};

char *data_getname(enum datatypes type, unsigned id, unsigned offset);

int data_getindexoffset(enum datatypes type, char *str, unsigned person);

void data_loadfile(enum datatypes type, char *file);

void data_loadfilesfromparams(struct params *param);

void data_cleanup(void);

#endif

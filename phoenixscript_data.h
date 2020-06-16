#ifndef PHOENIXSCRIPT_DATA_H
#define PHOENIXSCRIPT_DATA_H

#define sizeofarr(a) (sizeof(a) / sizeof(a[0]))
#define isvalidarrentry(ele, arr) ((ele) < sizeofarr(arr) && arr[ele])

extern char *speakers[4][60];
extern char *colors[4];
extern char *shiftdirection[4];
extern char *backgrounds[4][4096];
extern char *musicfading[2];
extern char *musicpause[2];
extern char *soundplay[2];
extern char *showside[2];
extern char *testimonypress[2];
extern char *fademode[6];
extern char *animationstate[2];
extern char *sound_data[4][1024];
extern char *locations[4][256];

extern char *cmd35flaghint[2];
extern char *cmd35jumphint[2];
extern char *bgshift[2];
extern char *personplacement[4];

extern char *personanimations[4][1024];

#endif

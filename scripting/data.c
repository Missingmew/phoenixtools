#include "data.h"
/* textual representation of constants found in the scripts */

/*	names_for_the_"PERSON"_command
	note_that_all_typos_and/or_inconsistencies_are_transcribed_as-is_from_the_US_releases
	TODO:_fix_these_names, maybe_add_localization? */

/* zeroes are required because apollo introduces 5 additional slots */
char *personplacement[4] = {
	"PLACE_DEFAULT", "PLACE_RIGHT", "PLACE_LEFT", "PLACE_UNK"
};

char *colors[4] = {
	"COLOR_WHITE", "COLOR_RED", "COLOR_BLUE", "COLOR_GREEN"
};

char *shiftdirection[4] = {
	"SHIFT_LEFT", "SHIFT_RIGHT", "SHIFT_UP", "SHIFT_DOWN"
};

char *bgshift[2] = {
	"BG_NOSHIFT", "BG_SHIFT"
};

char *cmd35flaghint[2] = {
	"SKIP_IF_FLAG_SET", "SKIP_IF_FLAG_UNSET"
};

char *cmd35jumphint[2] = {
	"JUMP_THIS_SECTION", "JUMP_OTHER_SECTION"
};

/* multilang backgrounds share their number! */
/* TODO: add support for multiple games */
/* 4096 is the size per game due to BGSPC, this may change due to various weird leftover commands though */
char *musicfading[2] = {
	"FADE_OUT", "FADE_IN"
};

char *musicpause[2] = {
	"MUSIC_PAUSE", "MUSIC_PLAY"
};

char *soundplay[2] = {
	"SOUND_STOP", "SOUND_START"
};

char *showside[2] = {
	"SHOW_LEFT", "SHOW_RIGHT"
};

char *testimonypress[2] = {
	"SHOWBOX", "HIDEBOX"
};

char *fademode[6] = {
	"FADE_NONE", "FADE_FROM_BLACK", "FADE_TO_BLACK", "FADE_FROM_WHITE", "FADE_TO_WHITE", "FADE_UNK5"
};

char *animationstate[2] = {
	"ANIM_STOP", "ANIM_START"
};


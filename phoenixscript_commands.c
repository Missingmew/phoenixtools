#include <stdio.h>

#include "phoenixscript_commands.h"
#include "phoenixscript_data.h"

#define sizeofarr(a) (sizeof(a) / sizeof(a[0]))

/* structure and prettyprinting functions for commands as presented in the GBA/NDS games */

/* all jump commands have the target encoded as _section numbers_ with 128 added on top */
/* personvanish arg1:
	03 - show
	04 - hide
*/
/* numbers in comments are arguments as stated in MessageSystem from unity */
command opcodeList[144] = {
	{ "noop", 0, printCmdGeneric },						/* does nothing */
	{ "linebreak", 0, printCmdGeneric },						/* linebreak */
	{ "pagebreak", 0, printCmdGeneric },						/* paragraph, ends current textbox, waits for player interaction */
	{ "textcolor", 1, printCmdColor },					/* text color, args: 0 white, 1 red, 2 blue, 3 green */
	{ "pause", 0, printCmdGeneric },	/* 1 */					/* pause the game, waits for player interaction */
	{ "music", 2, printCmdAudio },						/* change the music, args sequence(SDAT index) - fadein time frames (on resume only?) */
	{ "sound", 2, printCmdAudio },						/* play a sound effect, args ? - ? */
	{ "fullscreen_text", 0, printCmdGeneric },				/* switches to fullscreen display, GBA only? */
	{ "finger_choice_2_args_jmp", 2, printCmdChoice },			/* select between two choices (either in FS mode with previous opcode or from touchscreen), args: pointers to respective choices in script, followed by endjump? */
	{ "finger_choice_3_args_jmp", 3, printCmdChoice },			/* see above with 3 choices */
	{ "rejmp", 1, printCmdJump },						/* pointer to jump to for multiple choice questions failed once */
	{ "speed", 1, printCmdGeneric },						/* change text speed, args: frames/character */
	{ "wait", 1, printCmdGeneric },						/* wait for specified time units, args: frames to wait */
	{ "endjmp", 0, printCmdGeneric },					/* terminates a jump, usually found after all other jumpstatements */
	{ "name", 1, printCmdName },						/* change the name in the top left of a textbox, apparently, arg needs to be shifted to the right by 8 (actual value in upper 8 bits of 16bit arg) */
	{ "testimony_box", 2, printCmdGeneric },					/* begins a testimony section, args: ? - ? */
	{ "cmd10", 1 , printCmdGeneric },						/* ? */
	{ "evidence_window_plain", 0, printCmdGeneric },				/* show evidence window without lifebar (as when pressing R) */
	{ "bgcolor", 3, printCmdGeneric },					/* flashes the screen? args(?) color, fadein, fadeout in frames? args in python not clear enough :/ */
	{ "showevidence", 1, printCmdGeneric },					/* displays little evidence box with SFX, args: evidence to show? */
	{ "removeevidence", 0, printCmdGeneric },				/* removes above box with SFX */
	{ "special_jmp", 0, printCmdGeneric },					/* used at the end of testimony boxes for saves and resets? */
	{ "savegame", 0, printCmdGeneric },					/* displays saving screen */
	{ "newevidence", 1, printCmdGeneric },					/* adds evidence to court record with animation and SFX, args: object to add */
	{ "newevidence_noanim", 1, printCmdGeneric },				/* plays new evidence sound with nothing else?, args: ? */
	{ "cmd19", 2, printCmdGeneric },						/* ? */
	{ "swoosh", 4, printCmdGeneric },					/* starts panning the camera from one side of court to the other (always followed by wait 1e?), args: ? */
	{ "bg", 1, printCmdBackground },						/* change background image, args: background to display */
	{ "hidetextbox", 1, printCmdGeneric },					/* show textbox(border?), args: 0 show, 1 hide */
	{ "shift_background", 1, printCmdShiftBackground },				/* shifts background?, args: x*256 for direction + pixels/frame */
	{ "person", 3, printCmdPerson },					/* change the character image shown on-screen, args: ?,?,? */
	{ "hideperson", 0, printCmdGeneric },					/* instantly hide the character image */
	{ "cmd20", 1, printCmdGeneric },						/* ? */
	{ "evidence_window_lifebar", 0, printCmdGeneric },			/* show evidence window with lifebar */
	{ "fademusic", 2, printCmdFademusic },					/* fades music, args: fadeout/in, frames until 0/full volume */
	{ "cmd23", 2, printCmdGeneric },						/* ? */
	{ "reset", 0, printCmdGeneric },						/* reset the game to title screen */
	{ "cmd25", 1, printCmdGeneric },						/* ? */
	{ "hide_court_record_button", 1, printCmdGeneric },			/* shows court-record button, args: 0 show, 1 hide */
	{ "shake", 2, printCmdGeneric },						/* shakes the screen?, args: ? - ? (first seems to be 1e only and second is changing?) */
	{ "testemony_animation", 1, printCmdGeneric },				/* display "testimony" animation shown before witness testifies, args: ? */
	{ "return_to_testimony", 1, printCmdGeneric },				/* returns from a wrong answer to the point in testimony, where objection was called?, args: ? */
	{ "cmd2A", 3, printCmdGeneric },						/* ?, always followed by endjmp? */
	{ "cmd2B", 0, printCmdGeneric },						/* ? */
	{ "jmp", 1, printCmdJump },						/* jumps to pointer, args: pointer to target */
	{ "nextpage_button", 0, printCmdGeneric },				/* same as p */
	{ "nextpage_nobutton", 0, printCmdGeneric },				/* paragraph, ends textbox, no interaction */
	{ "animation", 2, printCmdAnimation },					/* display animation (such as "objection!"), args: ? */
	{ "cmd30", 1, printCmdGeneric },						/* ? */
	{ "personvanish", 2, printCmdGeneric },					/* makes characer vanish, args: ? */
	{ "cmd32", 2, printCmdGeneric },						/* ? */
	{ "cmd33", 2, printCmdGeneric },	/* 5 */					/* unknown jump, args: ? */
	{ "fadetoblack", 1, printCmdGeneric },					/* fades to black, args: ? */
	{ "cmd35", 2, printCmdGeneric },						/* ? */
	{ "cmd36", 0, printCmdGeneric },	/* 1 */					/* ?, unclear description in python */
	{ "cmd37", 2, printCmdGeneric },						/* ? */
	{ "cmd38", 1, printCmdGeneric },						/* ? */
	{ "littlesprite", 1, printCmdGeneric },					/* makes blip for characters on map in case 4 appear?, args: blip to show? */
	{ "cmd3A", 2, printCmdGeneric }, /* 3 */					/* ? */
	{ "cmd3B", 2, printCmdGeneric },						/* Animation related (begin animation?) */
	{ "cmd3C", 1, printCmdGeneric },						/* Animation related (makes argument blip flash?) */
	{ "cmd3D", 1, printCmdGeneric },						/* Animation related (stop animation?) */
	{ "cmd3E", 1, printCmdGeneric },						/* ? */
	{ "cmd3F", 0, printCmdGeneric },						/* unknown jump, args: ? */
	{ "cmd40", 0, printCmdGeneric },						/* ? */
	{ "cmd41", 0, printCmdGeneric },						/* ? */
	{ "togglevoicesfx", 1, printCmdGeneric },				/* toggles play of voice/typewriter sfx, args: 0 play - 1 stop */
	{ "show_lifebar", 1, printCmdGeneric },					/* show lifebar (with animation), args: 0 slide out - 1 slide in? */
	{ "guilty", 1, printCmdGeneric },					/* play guilty animation, args: ? */
	{ "cmd45", 0, printCmdGeneric },						/* jump at the end of special messages? */
	{ "bgtile", 1, printCmdGeneric },					/* change all background tiles, args: tile to change to */
	{ "cmd47", 2, printCmdGeneric },						/* ? */
	{ "cmd48", 2, printCmdGeneric },						/* ? */
	{ "wingame", 0, printCmdGeneric },					/* return to title, unlock all cases */
	{ "cmd4A", 0, printCmdGeneric }, /* 1 */					/* ?, crash? */
	{ "cmd4B", 1, printCmdGeneric },						/* ? */
	{ "cmd4C", 0, printCmdGeneric },						/* ? */
	{ "cmd4D", 2, printCmdGeneric },						/* ?, not tested? */
	{ "wait_noanim", 1, printCmdGeneric },					/* wait for specified time, no character animation, args: time to wait */
	{ "cmd4F", 7, printCmdGeneric },						/* ? */
	{ "cmd50", 1, printCmdGeneric },						/* ? */
	{ "cmd51", 2, printCmdGeneric },						/* ? */
	{ "cmd52", 1, printCmdGeneric },						/* ? */
	{ "cmd53", 0, printCmdGeneric },						/* ? */
	{ "lifebarset", 2, printCmdGeneric },					/* various lifebar related things?, args: ?, ? */
	{ "cmd55", 2, printCmdGeneric }, /* 1 */					/* ?, crash? */
	{ "cmd56", 2, printCmdGeneric },						/* ? */
	{ "psychoblock", 1, printCmdGeneric },					/* play psychoblock chain and lock appearance animation, args: locks to show */
	{ "cmd58", 0, printCmdGeneric },						/* ? */
	{ "cmd59", 1, printCmdGeneric },						/* ? */
	{ "cmd5A", 1, printCmdGeneric },						/* ? */
	{ "cmd5B", 2, printCmdGeneric },						/* ? */
	{ "cmd5C", 0, printCmdGeneric }, /* 3 */					/* ?, crash? */
	{ "toggle_center_text", 1, printCmdGeneric }, /* 0 */			/* toggles text centering, args: 0 normal alignment - 1 centered */
	{ "cmd5E", 1, printCmdGeneric }, /* 0 */					/* ? */
	{ "cmd5F", 3, printCmdGeneric },						/* ? */
	{ "cmd60", 0, printCmdGeneric }, /* 4 */					/* ?, crash? */
	{ "cmd61", 3, printCmdGeneric },						/* ? */
	{ "cmd62", 0, printCmdGeneric },						/* ? */
	{ "cmd63", 0, printCmdGeneric },						/* ?, crash? */
	{ "cmd64", 1, printCmdGeneric },						/* show special effect?, args: ? */
	{ "cmd65", 2, printCmdGeneric },						/* ? */
	{ "cmd66", 2, printCmdGeneric }, /* 3 */					/* ? */
	{ "cmd67", 0, printCmdGeneric },						/* ? */
	{ "cmd68", 0, printCmdGeneric },						/* ? */
	{ "bganim", 2, printCmdGeneric }, /* 4 */				/* play fullscreen animation, args: ? */
	{ "switchscript", 1, printCmdGeneric },					/* loads new script and jumps to beginning, args: current case? (python is unclear) */
	{ "cmd6B", 3, printCmdGeneric },						/* Animation related (load animation? used for maps?) */
	{ "cmd6C", 1, printCmdGeneric }, /* 0 */					/* ? */
	{ "cmd6D", 1, printCmdGeneric },						/* ? */
	{ "cmd6E", 1, printCmdGeneric },						/* ? */
	{ "cmd6F", 1, printCmdGeneric },						/* ? */
	{ "cmd70", 3, printCmdGeneric },						/* ? */
	{ "cmd71", 3, printCmdGeneric },						/* ? */
	{ "cmd72", 0, printCmdGeneric },						/* ? */
	{ "cmd73", 0, printCmdGeneric },						/* ?, crash? */
	{ "cmd74", 0, printCmdGeneric }, /* 2 */					/* ?, crash? */
	{ "cmd75", 4, printCmdGeneric },						/* ?, crash? */
	{ "cmd76", 0, printCmdGeneric }, /* 2 */					/* ?, crash? */
	{ "cmd77", 0, printCmdGeneric }, /* 2 */					/* ?, crash? */
	{ "cmd78", 0, printCmdGeneric }, /* 1 */					/* ?, crash? */
	{ "cmd79", 0, printCmdGeneric },						/* ?, crash? */
	{ "cmd7A", 0, printCmdGeneric }, /* 1 */					/* reset to capcom animation? */
	{ "cmd7B", 0, printCmdGeneric }, /* 2 */					/* ?, crash? */
	{ "cmd7C", 0, printCmdGeneric },						/* ?, crash? */
	{ "cmd7D", 0, printCmdGeneric }, /* 1 */					/* reset to capcom animation? */
	{ "cmd7E", 0, printCmdGeneric }, /* 1 */					/* ?, crash? */
	{ "cmd7F", 0, printCmdGeneric }, /* 1 */					/* ?, crash? */
	{ "cmd80", 0, printCmdGeneric },						/* dummy for apollotesting */
	{ "cmd81", 0, printCmdGeneric },						/* dummy for apollotesting */
	{ "cmd82", 0, printCmdGeneric },						/* dummy for apollotesting */
	{ "cmd83", 0, printCmdGeneric },						/* dummy for apollotesting */
	{ "cmd84", 0, printCmdGeneric },						/* dummy for apollotesting */
	{ "cmd85", 0, printCmdGeneric },						/* dummy for apollotesting */
	{ "cmd86", 0, printCmdGeneric },						/* dummy for apollotesting */
	{ "cmd87", 0, printCmdGeneric },						/* dummy for apollotesting */
	{ "cmd88", 0, printCmdGeneric },						/* dummy for apollotesting */
	{ "cmd89", 0, printCmdGeneric },						/* dummy for apollotesting */
	{ "cmd8A", 0, printCmdGeneric },						/* dummy for apollotesting */
	{ "cmd8B", 0, printCmdGeneric },						/* dummy for apollotesting */
	{ "cmd8C", 0, printCmdGeneric },						/* dummy for apollotesting */
	{ "cmd8D", 0, printCmdGeneric },						/* dummy for apollotesting */
	{ "cmd8E", 0, printCmdGeneric },						/* dummy for apollotesting */
	{ "cmd8F", 0, printCmdGeneric }						/* dummy for apollotesting */
};

/*
int printCmd(char *text, uint16_t *args, command *op) {
	int retval;
	
	return retval;
}
*/

int printCmdGeneric(char *text, uint16_t *args, command *op) {
	int retval = 0, i;
	retval += sprintf(text, "%s", op->name);
	if(op->args) {
		retval += sprintf(text+retval, " %05u", args[0]);
		for(i = 1; i < op->args; i++) retval += sprintf(text+retval, ", %05u", args[i]);
	}
	retval += sprintf(text+retval, "\n");
	return retval;
}

int printCmdColor(char *text, uint16_t *args, command *op) {
	if(args[0] < 4) return sprintf(text, "%s \"%s\"\n", op->name, colors[args[0]]);
	else return printCmdGeneric(text, args, op);
}

int printCmdName(char *text, uint16_t *args, command *op) {
	/* the bitshift is needed cause capcom seems to store the person in the upper 8 bits of the 16bit argument...
	   removed check if argument 1 is less then 55 because it seems to be ok and apollo exceeds this by 5 :/ */
	args[0] >>= 8;
	if(args[0] < sizeofarr(speakers[gamenum]) && speakers[gamenum][args[0]]) {
		return sprintf( text, "%s \"%s\"\n", op->name, speakers[gamenum][args[0]] );
	}
	else return printCmdGeneric(text, args, op);
}

int printCmdBackground(char *text, uint16_t *args, command *op) {
	if(args[0] < sizeofarr(backgrounds[gamenum]) && backgrounds[gamenum][args[0]]) {
		return sprintf( text, "%s \"%s\"\n", op->name, backgrounds[gamenum][args[0]] );
	}
	else return printCmdGeneric(text, args, op);
}

int printCmdJump(char *text, uint16_t *args, command *op) {
	return sprintf(text, "%s %05u\n", op->name, args[0]-128);
}

int printCmdShiftBackground(char *text, uint16_t *args, command *op) {
	return sprintf(text, "%s %s, %04u\n", op->name, shiftdirection[args[0]/256], args[0] % 256);
}

int printCmdPerson(char *text, uint16_t *args, command *op) {
	int retval = 0, i;
	
	if(args[0] < sizeofarr(speakers[gamenum]) && speakers[gamenum][args[0]]) {
		retval += sprintf(text, "%s \"%s\"", op->name, speakers[gamenum][args[0]]);
		for(i = 1; i < op->args; i++) retval += sprintf(text+retval, ", %05u", args[i]);
		retval += sprintf(text+retval, "\n");
	}
	else retval = printCmdGeneric(text, args, op);
	
	return retval;
}

int printCmdFademusic(char *text, uint16_t *args, command *op) {
	int retval = 0, i;
	
	if(args[0] < sizeofarr(musicfading)) {
		retval += sprintf(text, "%s \"%s\"", op->name, musicfading[args[0]]);
		for(i = 1; i < op->args; i++) retval += sprintf(text+retval, ", %05u", args[i]);
		retval += sprintf(text+retval, "\n");
	}
	else retval = printCmdGeneric(text, args, op);
	
	return retval;
}

int printCmdChoice(char *text, uint16_t *args, command *op) {
	int retval = 0, i;
	for(i = 0; i < op->args; i++) args[i] -= 128;
	
	retval += sprintf(text, "%s %05u", op->name, args[0]);
	for(i = 1; i < op->args; i++) retval += sprintf(text+retval, ", %05u", args[i]);
	retval += sprintf(text+retval, "\n");
	
	return retval;
}

int printCmdAudio(char *text, uint16_t *args, command *op) {
	int retval = 0, i;
	
	if(args[0] < sizeofarr(sound_data[gamenum]) && sound_data[gamenum][args[0]]) {
		retval += sprintf(text, "%s \"%s\"", op->name, sound_data[gamenum][args[0]]);
		for(i = 1; i < op->args; i++) retval += sprintf(text+retval, ", %05u", args[i]);
		retval += sprintf(text+retval, "\n");
	}
	else retval = printCmdGeneric(text, args, op);
	
	return retval;
}

int printCmdAnimation(char *text, uint16_t *args, command *op) {
	if(args[1] < sizeofarr(animationstate)) {
		return sprintf(text, "%s %05u, %s\n", op->name, args[0], animationstate[args[1]]);
	}
	else return printCmdGeneric(text, args, op);
}

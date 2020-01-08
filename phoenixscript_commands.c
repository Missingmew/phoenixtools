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

void printCmdGeneric(struct scriptstate *state, unsigned args) {
	int i;
	state->textidx += sprintf(state->textfile+state->textidx, "%s", commands[state->script[state->scriptidx]].name);
	state->scriptidx++;
	if(args) {
		state->textidx += sprintf(state->textfile+state->textidx, " %05u", state->script[state->scriptidx]);
		for(i = 1; i < args; i++) state->textidx += sprintf(state->textfile+state->textidx, ", %05u", state->script[state->scriptidx+i]);
		state->scriptidx += args;
	}
	state->textidx += sprintf(state->textfile+state->textidx, "\n");
}

void printCmd00(struct scriptstate *state) {
	printCmdGeneric(state, 0);
}

void printCmd01(struct scriptstate *state) {
	if(state->outidx) { // append linebreak to text if available
		state->textidx--;
		state->textfile[state->textidx] = 0;
		state->outidx += sprintf(state->outbuf+state->outidx, "\n");
		state->scriptidx++;
	}
	else printCmdGeneric(state, 0);
}

void printCmd02(struct scriptstate *state) {
	printCmdGeneric(state, 0);
}

void printCmd03(struct scriptstate *state) {
	if(state->script[state->scriptidx+1] < 4) {
		state->textidx += sprintf(state->textfile+state->textidx, "%s \"%s\"\n", commands[state->script[state->scriptidx]].name, colors[state->script[state->scriptidx+1]]);
		state->scriptidx += 1+1;
	}
	else printCmdGeneric(state, 1);
}

void printCmd04(struct scriptstate *state) { /* 1 */
	printCmdGeneric(state, 0);
}

void printCmd05(struct scriptstate *state) {
	if(state->script[state->scriptidx+1] < sizeofarr(sound_data[gamenum]) && sound_data[gamenum][state->script[state->scriptidx+1]]) {
		state->textidx += sprintf(state->textfile+state->textidx, "%s \"%s\", %05u\n", commands[state->script[state->scriptidx]].name, sound_data[gamenum][state->script[state->scriptidx+1]], state->script[state->scriptidx+2]);
		state->scriptidx += 1+2;
	}
	else printCmdGeneric(state, 2);
}

void printCmd06(struct scriptstate *state) {
	printCmd05(state);
}

void printCmd07(struct scriptstate *state) {
	printCmdGeneric(state, 0);
}

void printCmd08(struct scriptstate *state) {
	state->textidx += sprintf(state->textfile+state->textidx, "%s %05u, %05u\n", commands[state->script[state->scriptidx]].name, state->script[state->scriptidx+1]-128, state->script[state->scriptidx+2]-128);
	state->scriptidx += 1+2;
}

void printCmd09(struct scriptstate *state) {
	state->textidx += sprintf(state->textfile+state->textidx, "%s %05u, %05u, %05u\n", commands[state->script[state->scriptidx]].name, state->script[state->scriptidx+1]-128, state->script[state->scriptidx+2]-128, state->script[state->scriptidx+3]-128);
	state->scriptidx += 1+3;
}

void printCmd0A(struct scriptstate *state) {
	state->textidx += sprintf(state->textfile+state->textidx, "%s %05u\n", commands[state->script[state->scriptidx]].name, state->script[state->scriptidx+1]-128);
	state->scriptidx += 1+1;
}

void printCmd0B(struct scriptstate *state) {
	printCmdGeneric(state, 1);
}

void printCmd0C(struct scriptstate *state) {
	printCmdGeneric(state, 1);
}

void printCmd0D(struct scriptstate *state) {
	printCmdGeneric(state, 0);
}

void printCmd0E(struct scriptstate *state) {
	/* the bitshift is needed cause capcom seems to store the person in the upper 8 bits of the 16bit argument...
	   removed check if argument 1 is less then 55 because it seems to be ok and apollo exceeds this by 5 :/ */
	if((state->script[state->scriptidx+1] >> 8) < sizeofarr(speakers[gamenum]) && speakers[gamenum][(state->script[state->scriptidx+1] >> 8)]) {
		state->textidx += sprintf(state->textfile+state->textidx, "%s \"%s\"\n", commands[state->script[state->scriptidx]].name, speakers[gamenum][(state->script[state->scriptidx+1] >> 8)] );
		state->scriptidx += 1+1;
	}
	else printCmdGeneric(state, 1);
}

void printCmd0F(struct scriptstate *state) {
	printCmdGeneric(state, 2);
}

void printCmd10(struct scriptstate *state) { // flagctl todo
	printCmdGeneric(state, 1);
}

void printCmd11(struct scriptstate *state) {
	printCmdGeneric(state, 0);
}

void printCmd12(struct scriptstate *state) {
	printCmdGeneric(state, 3);
}

void printCmd13(struct scriptstate *state) {
	printCmdGeneric(state, 1);
}

void printCmd14(struct scriptstate *state) {
	printCmdGeneric(state, 0);
}

void printCmd15(struct scriptstate *state) {
	printCmdGeneric(state, 0);
}

void printCmd16(struct scriptstate *state) {
	printCmdGeneric(state, 0);
}

void printCmd17(struct scriptstate *state) {
	printCmdGeneric(state, 1);
}

void printCmd18(struct scriptstate *state) {
	printCmdGeneric(state, 1);
}

void printCmd19(struct scriptstate *state) {
	printCmdGeneric(state, 2);
}

void printCmd1A(struct scriptstate *state) {
	printCmdGeneric(state, 4);
}

void printCmd1B(struct scriptstate *state) {
	if(state->script[state->scriptidx+1] < sizeofarr(backgrounds[gamenum]) && backgrounds[gamenum][state->script[state->scriptidx+1]]) {
		state->textidx += sprintf(state->textfile+state->textidx, "%s \"%s\"\n", commands[state->script[state->scriptidx]].name, backgrounds[gamenum][state->script[state->scriptidx+1]] );
		state->scriptidx += 1+1;
	}
	else printCmdGeneric(state, 1);
}

void printCmd1C(struct scriptstate *state) {
	printCmdGeneric(state, 1);
}

void printCmd1D(struct scriptstate *state) {
	state->textidx += sprintf(state->textfile+state->textidx, "%s %s, %04u\n", commands[state->script[state->scriptidx]].name, shiftdirection[state->script[state->scriptidx+1]/256], state->script[state->scriptidx+1] % 256);
	state->scriptidx += 1+1;
}

void printCmd1E(struct scriptstate *state) {
	if(state->script[state->scriptidx+1] < sizeofarr(speakers[gamenum]) && speakers[gamenum][state->script[state->scriptidx+1]]) {
		state->textidx += sprintf(state->textfile+state->textidx, "%s \"%s\", %05u, %05u\n", commands[state->script[state->scriptidx]].name, speakers[gamenum][state->script[state->scriptidx+1]], state->script[state->scriptidx+2], state->script[state->scriptidx+3]);
		state->scriptidx += 1+3;
	}
	else printCmdGeneric(state, 3);
}

void printCmd1F(struct scriptstate *state) {
	printCmdGeneric(state, 0);
}

void printCmd20(struct scriptstate *state) {
	printCmdGeneric(state, 1);
}

void printCmd21(struct scriptstate *state) {
	printCmdGeneric(state, 0);
}

void printCmd22(struct scriptstate *state) {
	if(state->script[state->scriptidx+1] < sizeofarr(musicfading)) {
		state->textidx += sprintf(state->textfile+state->textidx, "%s \"%s\", %05u\n", commands[state->script[state->scriptidx]].name, musicfading[state->script[state->scriptidx+1]], state->script[state->scriptidx+2]);
		state->scriptidx += 1+2;
	}
	else printCmdGeneric(state, 2);
}

void printCmd23(struct scriptstate *state) {
	printCmdGeneric(state, 2);
}

void printCmd24(struct scriptstate *state) {
	printCmdGeneric(state, 0);
}

void printCmd25(struct scriptstate *state) {
	printCmdGeneric(state, 1);
}

void printCmd26(struct scriptstate *state) {
	printCmdGeneric(state, 1);
}

void printCmd27(struct scriptstate *state) {
	printCmdGeneric(state, 2);
}

void printCmd28(struct scriptstate *state) {
	printCmdGeneric(state, 1);
}

void printCmd29(struct scriptstate *state) {
	printCmdGeneric(state, 1);
}

void printCmd2A(struct scriptstate *state) {
	printCmdGeneric(state, 3);
}

void printCmd2B(struct scriptstate *state) {
	printCmdGeneric(state, 0);
}

void printCmd2C(struct scriptstate *state) {
	printCmd0A(state);
}

void printCmd2D(struct scriptstate *state) {
	printCmdGeneric(state, 0);
}

void printCmd2E(struct scriptstate *state) {
	printCmdGeneric(state, 0);
}

void printCmd2F(struct scriptstate *state) {
	if(state->script[state->scriptidx+2] < sizeofarr(animationstate)) {
		state->textidx += sprintf(state->textfile+state->textidx, "%s %05u, %s\n", commands[state->script[state->scriptidx]].name, state->script[state->scriptidx+1], animationstate[state->script[state->scriptidx+2]]);
		state->scriptidx += 1+2;
	}
	else printCmdGeneric(state, 2);
}

void printCmd30(struct scriptstate *state) {
	printCmdGeneric(state, 1);
}

void printCmd31(struct scriptstate *state) {
	printCmdGeneric(state, 2);
}

void printCmd32(struct scriptstate *state) {
	printCmdGeneric(state, 2);
}

void printCmd33(struct scriptstate *state) { /* 5 */
	printCmdGeneric(state, 2);
}

void printCmd34(struct scriptstate *state) {
	printCmdGeneric(state, 1);
}

void printCmd35(struct scriptstate *state) {
	printCmdGeneric(state, 2);
}

void printCmd36(struct scriptstate *state) {
	printCmdGeneric(state, 1);
}

void printCmd37(struct scriptstate *state) {
	printCmdGeneric(state, 2);
}

void printCmd38(struct scriptstate *state) {
	printCmdGeneric(state, 1);
}

void printCmd39(struct scriptstate *state) {
	printCmdGeneric(state, 1);
}

void printCmd3A(struct scriptstate *state) { /* 3 */
	printCmdGeneric(state, 2);
}

void printCmd3B(struct scriptstate *state) {
	printCmdGeneric(state, 2);
}

void printCmd3C(struct scriptstate *state) {
	printCmdGeneric(state, 1);
}

void printCmd3D(struct scriptstate *state) {
	printCmdGeneric(state, 1);
}

void printCmd3E(struct scriptstate *state) {
	printCmdGeneric(state, 1);
}

void printCmd3F(struct scriptstate *state) {
	printCmdGeneric(state, 0);
}

void printCmd40(struct scriptstate *state) {
	printCmdGeneric(state, 0);
}

void printCmd41(struct scriptstate *state) {
	printCmdGeneric(state, 0);
}

void printCmd42(struct scriptstate *state) {
	printCmdGeneric(state, 1);
}

void printCmd43(struct scriptstate *state) {
	printCmdGeneric(state, 1);
}

void printCmd44(struct scriptstate *state) {
	printCmdGeneric(state, 1);
}

void printCmd45(struct scriptstate *state) {
	printCmdGeneric(state, 0);
}

void printCmd46(struct scriptstate *state) {
	printCmdGeneric(state, 1);
}

void printCmd47(struct scriptstate *state) {
	printCmdGeneric(state, 2);
}

void printCmd48(struct scriptstate *state) {
	printCmdGeneric(state, 2);
}

void printCmd49(struct scriptstate *state) {
	printCmdGeneric(state, 0);
}

void printCmd4A(struct scriptstate *state) { /* 1 */
	printCmdGeneric(state, 0);
}

void printCmd4B(struct scriptstate *state) {
	printCmdGeneric(state, 1);
}

void printCmd4C(struct scriptstate *state) {
	printCmdGeneric(state, 0);
}

void printCmd4D(struct scriptstate *state) {
	printCmdGeneric(state, 2);
}

void printCmd4E(struct scriptstate *state) {
	printCmdGeneric(state, 1);
}

void printCmd4F(struct scriptstate *state) {
	printCmdGeneric(state, 7);
}

void printCmd50(struct scriptstate *state) {
	printCmdGeneric(state, 1);
}

void printCmd51(struct scriptstate *state) {
	printCmdGeneric(state, 2);
}

void printCmd52(struct scriptstate *state) {
	printCmdGeneric(state, 1);
}

void printCmd53(struct scriptstate *state) {
	printCmdGeneric(state, 0);
}

void printCmd54(struct scriptstate *state) {
	printCmdGeneric(state, 2);
}

void printCmd55(struct scriptstate *state) { /* 1 */
	printCmdGeneric(state, 2);
}

void printCmd56(struct scriptstate *state) {
	printCmdGeneric(state, 2);
}

void printCmd57(struct scriptstate *state) {
	printCmdGeneric(state, 1);
}

void printCmd58(struct scriptstate *state) {
	printCmdGeneric(state, 0);
}

void printCmd59(struct scriptstate *state) {
	printCmdGeneric(state, 1);
}

void printCmd5A(struct scriptstate *state) {
	printCmdGeneric(state, 1);
}

void printCmd5B(struct scriptstate *state) {
	printCmdGeneric(state, 2);
}

void printCmd5C(struct scriptstate *state) { /* 3 */
	printCmdGeneric(state, 0);
}

void printCmd5D(struct scriptstate *state) { /* 0 */
	printCmdGeneric(state, 1);
}

void printCmd5E(struct scriptstate *state) { /* 0 */
	printCmdGeneric(state, 1);
}

void printCmd5F(struct scriptstate *state) {
	printCmdGeneric(state, 3);
}

void printCmd60(struct scriptstate *state) { /* 4 */
	printCmdGeneric(state, 0);
}

void printCmd61(struct scriptstate *state) {
	printCmdGeneric(state, 3);
}

void printCmd62(struct scriptstate *state) {
	printCmdGeneric(state, 0);
}

void printCmd63(struct scriptstate *state) {
	printCmdGeneric(state, 0);
}

void printCmd64(struct scriptstate *state) {
	printCmdGeneric(state, 1);
}

void printCmd65(struct scriptstate *state) {
	printCmdGeneric(state, 2);
}

void printCmd66(struct scriptstate *state) { /* 3 */
	printCmdGeneric(state, 2);
}

void printCmd67(struct scriptstate *state) {
	printCmdGeneric(state, 0);
}

void printCmd68(struct scriptstate *state) {
	printCmdGeneric(state, 0);
}

void printCmd69(struct scriptstate *state) { /* 4 */
	printCmdGeneric(state, 2);
}

void printCmd6A(struct scriptstate *state) {
	printCmdGeneric(state, 1);
}

void printCmd6B(struct scriptstate *state) {
	printCmdGeneric(state, 3);
}

void printCmd6C(struct scriptstate *state) { /* 0 */
	printCmdGeneric(state, 1);
}

void printCmd6D(struct scriptstate *state) {
	printCmdGeneric(state, 1);
}

void printCmd6E(struct scriptstate *state) {
	printCmdGeneric(state, 1);
}

void printCmd6F(struct scriptstate *state) {
	printCmdGeneric(state, 1);
}

void printCmd70(struct scriptstate *state) {
	printCmdGeneric(state, 3);
}

void printCmd71(struct scriptstate *state) {
	printCmdGeneric(state, 3);
}

void printCmd72(struct scriptstate *state) {
	printCmdGeneric(state, 0);
}

void printCmd73(struct scriptstate *state) {
	printCmdGeneric(state, 0);
}

void printCmd74(struct scriptstate *state) { /* 2 */
	printCmdGeneric(state, 0);
}

void printCmd75(struct scriptstate *state) {
	printCmdGeneric(state, 4);
}

void printCmd76(struct scriptstate *state) { /* 2 */
	printCmdGeneric(state, 0);
}

void printCmd77(struct scriptstate *state) { /* 2 */
	printCmdGeneric(state, 0);
}

void printCmd78(struct scriptstate *state) { /* 1 */
	printCmdGeneric(state, 0);
}

void printCmd79(struct scriptstate *state) {
	printCmdGeneric(state, 0);
}

void printCmd7A(struct scriptstate *state) { /* 1 */
	printCmdGeneric(state, 0);
}

void printCmd7B(struct scriptstate *state) { /* 2 */
	printCmdGeneric(state, 0);
}

void printCmd7C(struct scriptstate *state) {
	printCmdGeneric(state, 0);
}

void printCmd7D(struct scriptstate *state) { /* 1 */
	printCmdGeneric(state, 0);
}

void printCmd7E(struct scriptstate *state) { /* 1 */
	printCmdGeneric(state, 0);
}

void printCmd7F(struct scriptstate *state) { /* 1 */
	printCmdGeneric(state, 0);
}

void printCmd80(struct scriptstate *state) {
	printCmdGeneric(state, 0);
}

void printCmd81(struct scriptstate *state) {
	printCmdGeneric(state, 0);
}

void printCmd82(struct scriptstate *state) {
	printCmdGeneric(state, 0);
}

void printCmd83(struct scriptstate *state) {
	printCmdGeneric(state, 0);
}

void printCmd84(struct scriptstate *state) {
	printCmdGeneric(state, 0);
}

void printCmd85(struct scriptstate *state) {
	printCmdGeneric(state, 0);
}

void printCmd86(struct scriptstate *state) {
	printCmdGeneric(state, 0);
}

void printCmd87(struct scriptstate *state) {
	printCmdGeneric(state, 0);
}

void printCmd88(struct scriptstate *state) {
	printCmdGeneric(state, 0);
}

void printCmd89(struct scriptstate *state) {
	printCmdGeneric(state, 0);
}

void printCmd8A(struct scriptstate *state) {
	printCmdGeneric(state, 0);
}

void printCmd8B(struct scriptstate *state) {
	printCmdGeneric(state, 0);
}

void printCmd8C(struct scriptstate *state) {
	printCmdGeneric(state, 0);
}

void printCmd8D(struct scriptstate *state) {
	printCmdGeneric(state, 0);
}

void printCmd8E(struct scriptstate *state) {
	printCmdGeneric(state, 0);
}

void printCmd8F(struct scriptstate *state) {
	printCmdGeneric(state, 0);
}
command commands[144] = {
	{ "cmd00", printCmd00 }, 			/* does something? */
	{ "linebreak", printCmd01 }, 			/* linebreak */
	{ "pagebreak", printCmd02 }, 			/* paragraph, ends current textbox, waits for player interaction */
	{ "textcolor", printCmd03 }, 			/* text color, args: 0 white, 1 red, 2 blue, 3 green */
	{ "pause", printCmd04 },			/* pause the game, waits for player interaction */
	{ "music", printCmd05 }, 			/* change the music, args sequence(SDAT index) - fadein time frames (on resume only?) */
	{ "sound", printCmd06 }, 			/* play a sound effect, args ? - ? */
	{ "fullscreen_text", printCmd07 }, 		/* switches to fullscreen display, GBA only? */
	{ "finger_choice_2_args_jmp", printCmd08 }, 	/* select between two choices (either in FS mode with previous opcode or from touchscreen), args: pointers to respective choices in script, followed by endjump? */
	{ "finger_choice_3_args_jmp", printCmd09 }, 	/* see above with 3 choices */
	{ "rejmp", printCmd0A }, 			/* pointer to jump to for multiple choice questions failed once */
	{ "speed", printCmd0B }, 			/* change text speed, args: frames/character */
	{ "wait", printCmd0C }, 			/* wait for specified time units, args: frames to wait */
	{ "endjmp", printCmd0D }, 			/* terminates a jump, usually found after all other jumpstatements */
	{ "name", printCmd0E }, 			/* change the name in the top left of a textbox, apparently, arg needs to be shifted to the right by 8 (actual value in upper 8 bits of 16bit arg) */
	{ "testimony_box", printCmd0F }, 		/* begins a testimony section, args: ? - ? */
	{ "flagctl" , printCmd10 },			/* modifies flags (set/unset) */
	{ "evidence_window_plain", printCmd11 }, 	/* show evidence window without lifebar (as when pressing R) */
	{ "bgcolor", printCmd12 }, 			/* flashes the screen? args(?) color, fadein, fadeout in frames? args in python not clear enough :/ */
	{ "showevidence", printCmd13 }, 		/* displays little evidence box with SFX, args: evidence to show? */
	{ "removeevidence", printCmd14 }, 		/* removes above box with SFX */
	{ "special_jmp", printCmd15 }, 			/* used at the end of testimony boxes for saves and resets? */
	{ "savegame", printCmd16 }, 			/* displays saving screen */
	{ "newevidence", printCmd17 }, 			/* adds evidence to court record with animation and SFX, args: object to add */
	{ "newevidence_noanim", printCmd18 }, 		/* plays new evidence sound with nothing else?, args: ? */
	{ "cmd19", printCmd19 }, 			/* ? */
	{ "swoosh", printCmd1A }, 			/* starts panning the camera from one side of court to the other (always followed by wait 1e?), args: ? */
	{ "bg", printCmd1B }, 				/* change background image, args: background to display */
	{ "hidetextbox", printCmd1C }, 			/* show textbox(border?), args: 0 show, 1 hide */
	{ "shift_background", printCmd1D }, 		/* shifts background?, args: x*256 for direction + pixels/frame */
	{ "person", printCmd1E }, 			/* change the character image shown on-screen, args: ?,?,? */
	{ "hideperson", printCmd1F }, 			/* instantly hide the character image */
	{ "cmd20", printCmd20 }, 			/* ? */
	{ "evidence_window_lifebar", printCmd21 }, 	/* show evidence window with lifebar */
	{ "fademusic", printCmd22 }, 			/* fades music, args: fadeout/in, frames until 0/full volume */
	{ "cmd23", printCmd23 }, 			/* ? */
	{ "reset", printCmd24 }, 			/* reset the game to title screen */
	{ "cmd25", printCmd25 }, 			/* ? */
	{ "hide_court_record_button", printCmd26 }, 	/* shows court-record button, args: 0 show, 1 hide */
	{ "shake", printCmd27 }, 			/* shakes the screen?, args: ? - ? (first seems to be 1e only and second is changing?) */
	{ "testemony_animation", printCmd28 }, 		/* display "testimony" animation shown before witness testifies, args: ? */
	{ "return_to_testimony", printCmd29 }, 		/* returns from a wrong answer to the point in testimony, where objection was called?, args: ? */
	{ "cmd2A", printCmd2A },  			/* ?, always followed by endjmp? */
	{ "cmd2B", printCmd2B },  			/* ? */
	{ "jmp", printCmd2C }, 				/* jumps to pointer, args: pointer to target */
	{ "nextpage_button", printCmd2D }, 		/* same as p */
	{ "nextpage_nobutton", printCmd2E }, 		/* paragraph, ends textbox, no interaction */
	{ "animation", printCmd2F }, 			/* display animation (such as "objection!"), args: ? */
	{ "cmd30", printCmd30 },  			/* ? */
	{ "personvanish", printCmd31 }, 		/* makes characer vanish, args: ? */
	{ "cmd32", printCmd32 },  			/* ? */
	{ "cmd33", printCmd33 },			/* unknown jump, args: ? */
	{ "fadetoblack", printCmd34 }, 			/* fades to black, args: ? */
	{ "cmd35", printCmd35 },  			/* ? */
	{ "cmd36", printCmd36 },			/* ?, unclear description in python */
	{ "cmd37", printCmd37 },  			/* ? */
	{ "cmd38", printCmd38 },  			/* ? */
	{ "littlesprite", printCmd39 }, 		/* makes blip for characters on map in case 4 appear?, args: blip to show? */
	{ "cmd3A", printCmd3A },			/* ? */
	{ "cmd3B", printCmd3B },  			/* Animation related (begin animation?) */
	{ "cmd3C", printCmd3C },  			/* Animation related (makes argument blip flash?) */
	{ "cmd3D", printCmd3D },  			/* Animation related (stop animation?) */
	{ "cmd3E", printCmd3E },  			/* ? */
	{ "cmd3F", printCmd3F },  			/* unknown jump, args: ? */
	{ "cmd40", printCmd40 },  			/* ? */
	{ "cmd41", printCmd41 },  			/* ? */
	{ "togglevoicesfx", printCmd42 }, 		/* toggles play of voice/typewriter sfx, args: 0 play - 1 stop */
	{ "show_lifebar", printCmd43 }, 		/* show lifebar (with animation), args: 0 slide out - 1 slide in? */
	{ "guilty", printCmd44 }, 			/* play guilty animation, args: ? */
	{ "cmd45", printCmd45 },  			/* jump at the end of special messages? */
	{ "bgtile", printCmd46 }, 			/* change all background tiles, args: tile to change to */
	{ "cmd47", printCmd47 },  			/* ? */
	{ "cmd48", printCmd48 },  			/* ? */
	{ "wingame", printCmd49 }, 			/* return to title, unlock all cases */
	{ "cmd4A", printCmd4A },			/* ?, crash? */
	{ "cmd4B", printCmd4B },  			/* ? */
	{ "cmd4C", printCmd4C },  			/* ? */
	{ "cmd4D", printCmd4D },  			/* ?, not tested? */
	{ "wait_noanim", printCmd4E }, 			/* wait for specified time, no character animation, args: time to wait */
	{ "cmd4F", printCmd4F },  			/* ? */
	{ "cmd50", printCmd50 },  			/* ? */
	{ "cmd51", printCmd51 },  			/* ? */
	{ "cmd52", printCmd52 },  			/* ? */
	{ "cmd53", printCmd53 },  			/* ? */
	{ "lifebarset", printCmd54 }, 			/* various lifebar related things?, args: ?, ? */
	{ "cmd55", printCmd55 },			/* ?, crash? */
	{ "cmd56", printCmd56 },  			/* ? */
	{ "psychoblock", printCmd57 }, 			/* play psychoblock chain and lock appearance animation, args: locks to show */
	{ "cmd58", printCmd58 },  			/* ? */
	{ "cmd59", printCmd59 },  			/* ? */
	{ "cmd5A", printCmd5A },  			/* ? */
	{ "cmd5B", printCmd5B },  			/* ? */
	{ "cmd5C", printCmd5C },			/* ?, crash? */
	{ "toggle_center_text", printCmd5D },		/* toggles text centering, args: 0 normal alignment - 1 centered */
	{ "cmd5E", printCmd5E },			/* ? */
	{ "cmd5F", printCmd5F },  			/* ? */
	{ "cmd60", printCmd60 },			/* ?, crash? */
	{ "cmd61", printCmd61 },  			/* ? */
	{ "cmd62", printCmd62 },  			/* ? */
	{ "cmd63", printCmd63 },  			/* ?, crash? */
	{ "cmd64", printCmd64 },  			/* show special effect?, args: ? */
	{ "cmd65", printCmd65 },  			/* ? */
	{ "cmd66", printCmd66 },			/* ? */
	{ "cmd67", printCmd67 },  			/* ? */
	{ "cmd68", printCmd68 },  			/* ? */
	{ "bganim", printCmd69 },			/* play fullscreen animation, args: ? */
	{ "switchscript", printCmd6A }, 		/* loads new script and jumps to beginning, args: current case? (python is unclear) */
	{ "cmd6B", printCmd6B },  			/* Animation related (load animation? used for maps?) */
	{ "cmd6C", printCmd6C },			/* ? */
	{ "cmd6D", printCmd6D },  			/* ? */
	{ "cmd6E", printCmd6E },  			/* ? */
	{ "cmd6F", printCmd6F },  			/* ? */
	{ "cmd70", printCmd70 },  			/* ? */
	{ "cmd71", printCmd71 },  			/* ? */
	{ "cmd72", printCmd72 },  			/* ? */
	{ "cmd73", printCmd73 },  			/* ?, crash? */
	{ "cmd74", printCmd74 },			/* ?, crash? */
	{ "cmd75", printCmd75 },  			/* ?, crash? */
	{ "cmd76", printCmd76 },			/* ?, crash? */
	{ "cmd77", printCmd77 },			/* ?, crash? */
	{ "cmd78", printCmd78 },			/* ?, crash? */
	{ "cmd79", printCmd79 },  			/* ?, crash? */
	{ "cmd7A", printCmd7A },			/* reset to capcom animation? */
	{ "cmd7B", printCmd7B },			/* ?, crash? */
	{ "cmd7C", printCmd7C },  			/* ?, crash? */
	{ "cmd7D", printCmd7D },			/* reset to capcom animation? */
	{ "cmd7E", printCmd7E },			/* ?, crash? */
	{ "cmd7F", printCmd7F }, 			/* ?, crash? */
	{ "cmd80", printCmd80 },  			/* dummy for apollotesting */
	{ "cmd81", printCmd81 },  			/* dummy for apollotesting */
	{ "cmd82", printCmd82 },  			/* dummy for apollotesting */
	{ "cmd83", printCmd83 },  			/* dummy for apollotesting */
	{ "cmd84", printCmd84 },  			/* dummy for apollotesting */
	{ "cmd85", printCmd85 },  			/* dummy for apollotesting */
	{ "cmd86", printCmd86 },  			/* dummy for apollotesting */
	{ "cmd87", printCmd87 },  			/* dummy for apollotesting */
	{ "cmd88", printCmd88 },  			/* dummy for apollotesting */
	{ "cmd89", printCmd89 },  			/* dummy for apollotesting */
	{ "cmd8A", printCmd8A },  			/* dummy for apollotesting */
	{ "cmd8B", printCmd8B },  			/* dummy for apollotesting */
	{ "cmd8C", printCmd8C },  			/* dummy for apollotesting */
	{ "cmd8D", printCmd8D },  			/* dummy for apollotesting */
	{ "cmd8E", printCmd8E },  			/* dummy for apollotesting */
	{ "cmd8F", printCmd8F }				/* dummy for apollotesting */
};

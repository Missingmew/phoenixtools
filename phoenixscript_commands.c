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

unsigned printCmdGeneric(struct scriptstate *state, unsigned args) {
	unsigned i;
	if(state->outputenabled) {
		state->textidx += sprintf(state->textfile+state->textidx, "%s", commands[state->script[state->scriptidx]].name);
		state->scriptidx++;
		if(args) {
			state->textidx += sprintf(state->textfile+state->textidx, " %05u", state->script[state->scriptidx]);
			for(i = 1; i < args; i++) state->textidx += sprintf(state->textfile+state->textidx, ", %05u", state->script[state->scriptidx+i]);
			state->scriptidx += args;
		}
		state->textidx += sprintf(state->textfile+state->textidx, "\n");
	}
	return args;
}

unsigned printCmd00(struct scriptstate *state) {
	return printCmdGeneric(state, 0);
}

unsigned printCmd01(struct scriptstate *state) {
	if(state->outputenabled) {
		if(state->outidx) { // append linebreak to text if available
			state->outidx += sprintf(state->outbuf+state->outidx, "\n");
			state->scriptidx++;
		}
		else return printCmdGeneric(state, 0);
	}
	return 0;
}

unsigned printCmd02(struct scriptstate *state) {
	/* the function in GS1 gba for this is a mess.
	   this appears to work however with no arguments so ill keep rolling with this for now. */
	return printCmdGeneric(state, 0);
}

unsigned printCmd03(struct scriptstate *state) {
	if(state->outputenabled) {
		if(state->script[state->scriptidx+1] < 4) {
			state->textidx += sprintf(state->textfile+state->textidx, "%s \"%s\"\n", commands[state->script[state->scriptidx]].name, colors[state->script[state->scriptidx+1]]);
			state->scriptidx += 1+1;
		}
		else return printCmdGeneric(state, 1);
	}
	return 1;
}

unsigned printCmd04(struct scriptstate *state) { /* 1 */
	/* unity says 1 arg, GS1 gba is either one or two args. zero works though so... */
	return printCmdGeneric(state, 0);
}

unsigned printCmd05(struct scriptstate *state) {
	if(state->outputenabled) {
		if(state->script[state->scriptidx+1] < sizeofarr(sound_data[ARRGAMENUM(state->gamenum)]) && sound_data[ARRGAMENUM(state->gamenum)][state->script[state->scriptidx+1]]) {
			state->textidx += sprintf(state->textfile+state->textidx, "%s \"%s\", %05u\n", commands[state->script[state->scriptidx]].name, sound_data[ARRGAMENUM(state->gamenum)][state->script[state->scriptidx+1]], state->script[state->scriptidx+2]);
			state->scriptidx += 1+2;
		}
		else return printCmdGeneric(state, 2);
	}
	return 2;
}

unsigned printCmd06(struct scriptstate *state) {
	/* GS1GBA only takes one argument for the sound command */
	if(state->gamenum == GAME_GS1GBA) {
		if(state->outputenabled) {
			unsigned int seNum = state->script[state->scriptidx+1] >> 8;
			if(seNum < sizeofarr(sound_data[ARRGAMENUM(state->gamenum)]) && sound_data[ARRGAMENUM(state->gamenum)][seNum]) {
				state->textidx += sprintf(state->textfile+state->textidx, "%s \"%s\", %05u\n", commands[state->script[state->scriptidx]].name, sound_data[ARRGAMENUM(state->gamenum)][seNum], state->script[state->scriptidx+1] & 1);
				state->scriptidx += 1+1;
			}
			else return printCmdGeneric(state, 1);
		}
		return 1;
	}
	else return printCmd05(state);
}

unsigned printCmd07(struct scriptstate *state) {
	return printCmdGeneric(state, 0);
}

unsigned printCmd08(struct scriptstate *state) {
	if(state->outputenabled) {
		state->textidx += sprintf(state->textfile+state->textidx, "%s %05u, %05u\n", commands[state->script[state->scriptidx]].name, state->script[state->scriptidx+1]-128, state->script[state->scriptidx+2]-128);
		state->scriptidx += 1+2;
	}
	return 2;
}

unsigned printCmd09(struct scriptstate *state) {
	if(state->outputenabled) {
		state->textidx += sprintf(state->textfile+state->textidx, "%s %05u, %05u, %05u\n", commands[state->script[state->scriptidx]].name, state->script[state->scriptidx+1]-128, state->script[state->scriptidx+2]-128, state->script[state->scriptidx+3]-128);
		state->scriptidx += 1+3;
	}
	return 3;
}

unsigned printCmd0A(struct scriptstate *state) {
	if(state->outputenabled) {
		state->textidx += sprintf(state->textfile+state->textidx, "%s %05u\n", commands[state->script[state->scriptidx]].name, state->script[state->scriptidx+1]-128);
		state->scriptidx += 1+1;
	}
	return 1;
}

unsigned printCmd0B(struct scriptstate *state) {
	return printCmdGeneric(state, 1);
}

unsigned printCmd0C(struct scriptstate *state) {
	return printCmdGeneric(state, 1);
}

unsigned printCmd0D(struct scriptstate *state) {
	return printCmdGeneric(state, 0);
}

unsigned printCmd0E(struct scriptstate *state) {
	/* the bitshift is needed cause capcom seems to store the person in the upper 8 bits of the 16bit argument...
	   removed check if argument 1 is less then 55 because it seems to be ok and apollo exceeds this by 5 :/ */
	if(state->outputenabled) {
		if((state->script[state->scriptidx+1] >> 8) < sizeofarr(speakers[ARRGAMENUM(state->gamenum)]) && speakers[ARRGAMENUM(state->gamenum)][(state->script[state->scriptidx+1] >> 8)]) {
			state->textidx += sprintf(state->textfile+state->textidx, "%s \"%s\"\n", commands[state->script[state->scriptidx]].name, speakers[ARRGAMENUM(state->gamenum)][(state->script[state->scriptidx+1] >> 8)] );
			state->scriptidx += 1+1;
		}
		else return printCmdGeneric(state, 1);
	}
	return 1;
}

unsigned printCmd0F(struct scriptstate *state) {
	return printCmdGeneric(state, 2);
}

unsigned printCmd10(struct scriptstate *state) {
	if(state->outputenabled) {
		unsigned num = state->script[state->scriptidx+1] & 0xFF;
		unsigned num2 = (state->script[state->scriptidx+1] & 0x7F00) >> 8;
		unsigned set = state->script[state->scriptidx+1] >> 15;
		state->textidx += sprintf(state->textfile+state->textidx, "%s %u, %u, %u\n", commands[state->script[state->scriptidx]].name, num2, num, set);
		state->scriptidx += 1+1;
	}
	return 1;
}

unsigned printCmd11(struct scriptstate *state) {
	return printCmdGeneric(state, 0);
}

unsigned printCmd12(struct scriptstate *state) {
	return printCmdGeneric(state, 3);
}

unsigned printCmd13(struct scriptstate *state) {
	return printCmdGeneric(state, 1);
}

unsigned printCmd14(struct scriptstate *state) {
	return printCmdGeneric(state, 0);
}

unsigned printCmd15(struct scriptstate *state) {
	return printCmdGeneric(state, 0);
}

unsigned printCmd16(struct scriptstate *state) {
	return printCmdGeneric(state, 0);
}

unsigned printCmd17(struct scriptstate *state) {
	return printCmdGeneric(state, 1);
}

unsigned printCmd18(struct scriptstate *state) {
	return printCmdGeneric(state, 1);
}

unsigned printCmd19(struct scriptstate *state) {
	return printCmdGeneric(state, 2);
}

unsigned printCmd1A(struct scriptstate *state) {
	return printCmdGeneric(state, 4);
}

unsigned printCmd1B(struct scriptstate *state) {
	if(state->outputenabled) {
		if(state->script[state->scriptidx+1] < sizeofarr(backgrounds[ARRGAMENUM(state->gamenum)]) && backgrounds[ARRGAMENUM(state->gamenum)][state->script[state->scriptidx+1]]) {
			state->textidx += sprintf(state->textfile+state->textidx, "%s \"%s\"\n", commands[state->script[state->scriptidx]].name, backgrounds[ARRGAMENUM(state->gamenum)][state->script[state->scriptidx+1]] );
			state->scriptidx += 1+1;
		}
		else return printCmdGeneric(state, 1);
	}
	return 1;
}

unsigned printCmd1C(struct scriptstate *state) {
	return printCmdGeneric(state, 1);
}

unsigned printCmd1D(struct scriptstate *state) {
	if(state->outputenabled) {
		state->textidx += sprintf(state->textfile+state->textidx, "%s %s, %04u\n", commands[state->script[state->scriptidx]].name, shiftdirection[state->script[state->scriptidx+1]/256], state->script[state->scriptidx+1] % 256);
		state->scriptidx += 1+1;
	}
	return 1;
}

unsigned printCmd1E(struct scriptstate *state) {
	if(state->outputenabled) {
		if(state->script[state->scriptidx+1] < sizeofarr(speakers[ARRGAMENUM(state->gamenum)]) && speakers[ARRGAMENUM(state->gamenum)][state->script[state->scriptidx+1]]) {
			state->textidx += sprintf(state->textfile+state->textidx, "%s \"%s\", %05u, %05u\n", commands[state->script[state->scriptidx]].name, speakers[ARRGAMENUM(state->gamenum)][state->script[state->scriptidx+1]], state->script[state->scriptidx+2], state->script[state->scriptidx+3]);
			state->scriptidx += 1+3;
		}
		else return printCmdGeneric(state, 3);
	}
	return 3;
}

unsigned printCmd1F(struct scriptstate *state) {
	return printCmdGeneric(state, 0);
}

unsigned printCmd20(struct scriptstate *state) {
	return printCmdGeneric(state, 1);
}

unsigned printCmd21(struct scriptstate *state) {
	return printCmdGeneric(state, 0);
}

unsigned printCmd22(struct scriptstate *state) {
	if(state->outputenabled) {
		if(state->script[state->scriptidx+1] < sizeofarr(musicfading)) {
			state->textidx += sprintf(state->textfile+state->textidx, "%s \"%s\", %05u\n", commands[state->script[state->scriptidx]].name, musicfading[state->script[state->scriptidx+1]], state->script[state->scriptidx+2]);
			state->scriptidx += 1+2;
		}
		else return printCmdGeneric(state, 2);
	}
	return 2;
}

unsigned printCmd23(struct scriptstate *state) {
	return printCmdGeneric(state, 2);
}

unsigned printCmd24(struct scriptstate *state) {
	return printCmdGeneric(state, 0);
}

unsigned printCmd25(struct scriptstate *state) {
	return printCmdGeneric(state, 1);
}

unsigned printCmd26(struct scriptstate *state) {
	return printCmdGeneric(state, 1);
}

unsigned printCmd27(struct scriptstate *state) {
	return printCmdGeneric(state, 2);
}

unsigned printCmd28(struct scriptstate *state) {
	return printCmdGeneric(state, 1);
}

unsigned printCmd29(struct scriptstate *state) {
	return printCmdGeneric(state, 1);
}

unsigned printCmd2A(struct scriptstate *state) {
	return printCmdGeneric(state, 3);
}

unsigned printCmd2B(struct scriptstate *state) {
	return printCmdGeneric(state, 0);
}

unsigned printCmd2C(struct scriptstate *state) {
	return printCmd0A(state);
}

unsigned printCmd2D(struct scriptstate *state) {
	return printCmdGeneric(state, 0);
}

unsigned printCmd2E(struct scriptstate *state) {
	return printCmdGeneric(state, 0);
}

unsigned printCmd2F(struct scriptstate *state) {
	if(state->outputenabled) {
		if(state->script[state->scriptidx+2] < sizeofarr(animationstate)) {
			state->textidx += sprintf(state->textfile+state->textidx, "%s %05u, %s\n", commands[state->script[state->scriptidx]].name, state->script[state->scriptidx+1], animationstate[state->script[state->scriptidx+2]]);
			state->scriptidx += 1+2;
		}
		else return printCmdGeneric(state, 2);
	}
	return 2;
}

unsigned printCmd30(struct scriptstate *state) {
	return printCmdGeneric(state, 1);
}

unsigned printCmd31(struct scriptstate *state) {
	return printCmdGeneric(state, 2);
}

unsigned printCmd32(struct scriptstate *state) {
	return printCmdGeneric(state, 2);
}

unsigned printCmd33(struct scriptstate *state) {
	unsigned i;
	if(state->outputenabled) {
		state->textidx += sprintf(state->textfile+state->textidx, "%s", commands[state->script[state->scriptidx]].name);
		
		if(state->script[state->scriptidx+1] < sizeofarr(locations[ARRGAMENUM(state->gamenum)]) && locations[ARRGAMENUM(state->gamenum)][state->script[state->scriptidx+1]]) {
			state->textidx += sprintf(state->textfile+state->textidx, " %s", locations[ARRGAMENUM(state->gamenum)][state->script[state->scriptidx+1]]);
		}
		else state->textidx += sprintf(state->textfile+state->textidx, " %u", state->script[state->scriptidx+1]);
		
		for(i = 2; i < 6; i++) {
			if(state->script[state->scriptidx+i] < sizeofarr(locations[ARRGAMENUM(state->gamenum)]) && locations[ARRGAMENUM(state->gamenum)][state->script[state->scriptidx+i]]) {
				state->textidx += sprintf(state->textfile+state->textidx, ", %s", locations[ARRGAMENUM(state->gamenum)][state->script[state->scriptidx+i]]);
			}
			else state->textidx += sprintf(state->textfile+state->textidx, ", %u", state->script[state->scriptidx+i]);
		}
		state->textidx += sprintf(state->textfile+state->textidx, "\n");
		state->scriptidx += 1+5;
	}
	return 5;
}

unsigned printCmd34(struct scriptstate *state) {
	return printCmdGeneric(state, 1);
}

unsigned printCmd35(struct scriptstate *state) {
	/* this needs more looking into. it involves special data to allow jumping at arbitrary offsets in other sections */
	unsigned targetsection, offset;
	if(state->outputenabled) {
		state->textidx += sprintf(state->textfile+state->textidx, "%s", commands[state->script[state->scriptidx]].name);
		state->textidx += sprintf(state->textfile+state->textidx, " %s, %u, %s, ", cmd35hints[0][state->script[state->scriptidx+1] & 1], state->script[state->scriptidx+1] >> 8, cmd35hints[1][(state->script[state->scriptidx+1] & 0x80) ? 1 : 0]);
		if(state->script[state->scriptidx+1] & 0x80) {
			offset = state->specialdata[(state->script[state->scriptidx+2] - state->numsections)].val0;
			targetsection = state->specialdata[(state->script[state->scriptidx+2] - state->numsections)].val1;
			state->textidx += sprintf(state->textfile+state->textidx, "%u + %u\n", targetsection, offset/2);
		}
		else {
			state->textidx += sprintf(state->textfile+state->textidx, "%u\n", state->script[state->scriptidx+2]/2);
		}
		state->scriptidx += 1+2;
	}
	return 2;
}

unsigned printCmd36(struct scriptstate *state) {
	unsigned targetsection, offset;
	if(state->outputenabled) {
		if((state->script[state->scriptidx+1] - state->numsections) < state->numspecialdata) {
			state->textidx += sprintf(state->textfile+state->textidx, "%s", commands[state->script[state->scriptidx]].name);
			//~ printf("cmd36 using index %08x(%08x)\n", (state->script[state->scriptidx+1] - state->numsections), state->script[state->scriptidx+1]);
			offset = state->specialdata[(state->script[state->scriptidx+1] - state->numsections)].val0;
			targetsection = state->specialdata[(state->script[state->scriptidx+1] - state->numsections)].val1;
			state->textidx += sprintf(state->textfile+state->textidx, " %u + %u\n", targetsection, offset/2);
			state->scriptidx += 1+1;
		}
		else {
			//~ printf("cmd36 invalid index! %08x(%08x)\n", (state->script[state->scriptidx+1] - state->numsections), state->script[state->scriptidx+1]);
			return printCmdGeneric(state, 1);
		}
	}
	return 1;
}

unsigned printCmd37(struct scriptstate *state) {
	return printCmdGeneric(state, 2);
}

unsigned printCmd38(struct scriptstate *state) {
	return printCmdGeneric(state, 1);
}

unsigned printCmd39(struct scriptstate *state) {
	return printCmdGeneric(state, 1);
}

unsigned printCmd3A(struct scriptstate *state) { /* 3 */
	/* has 2 args in GS1 gba */
	return printCmdGeneric(state, 2);
}

unsigned printCmd3B(struct scriptstate *state) {
	return printCmdGeneric(state, 2);
}

unsigned printCmd3C(struct scriptstate *state) {
	return printCmdGeneric(state, 1);
}

unsigned printCmd3D(struct scriptstate *state) {
	return printCmdGeneric(state, 1);
}

unsigned printCmd3E(struct scriptstate *state) {
	return printCmdGeneric(state, 1);
}

unsigned printCmd3F(struct scriptstate *state) {
	return printCmdGeneric(state, 0);
}

unsigned printCmd40(struct scriptstate *state) {
	return printCmdGeneric(state, 0);
}

unsigned printCmd41(struct scriptstate *state) {
	return printCmdGeneric(state, 0);
}

unsigned printCmd42(struct scriptstate *state) {
	return printCmdGeneric(state, 1);
}

unsigned printCmd43(struct scriptstate *state) {
	return printCmdGeneric(state, 1);
}

unsigned printCmd44(struct scriptstate *state) {
	return printCmdGeneric(state, 1);
}

unsigned printCmd45(struct scriptstate *state) {
	return printCmdGeneric(state, 0);
}

unsigned printCmd46(struct scriptstate *state) {
	return printCmdGeneric(state, 1);
}

unsigned printCmd47(struct scriptstate *state) {
	return printCmdGeneric(state, 2);
}

unsigned printCmd48(struct scriptstate *state) {
	return printCmdGeneric(state, 2);
}

unsigned printCmd49(struct scriptstate *state) {
	return printCmdGeneric(state, 0);
}

unsigned printCmd4A(struct scriptstate *state) { /* 1 */
	return printCmdGeneric(state, 0);
}

unsigned printCmd4B(struct scriptstate *state) {
	return printCmdGeneric(state, 1);
}

unsigned printCmd4C(struct scriptstate *state) {
	return printCmdGeneric(state, 0);
}

unsigned printCmd4D(struct scriptstate *state) {
	return printCmdGeneric(state, 2);
}

unsigned printCmd4E(struct scriptstate *state) {
	return printCmdGeneric(state, 1);
}

unsigned printCmd4F(struct scriptstate *state) {
	return printCmdGeneric(state, 7);
}

unsigned printCmd50(struct scriptstate *state) {
	return printCmdGeneric(state, 1);
}

unsigned printCmd51(struct scriptstate *state) {
	return printCmdGeneric(state, 2);
}

unsigned printCmd52(struct scriptstate *state) {
	return printCmdGeneric(state, 1);
}

unsigned printCmd53(struct scriptstate *state) {
	return printCmdGeneric(state, 0);
}

unsigned printCmd54(struct scriptstate *state) {
	return printCmdGeneric(state, 2);
}

unsigned printCmd55(struct scriptstate *state) { /* 1 */
	return printCmdGeneric(state, 2);
}

unsigned printCmd56(struct scriptstate *state) {
	return printCmdGeneric(state, 2);
}

unsigned printCmd57(struct scriptstate *state) {
	return printCmdGeneric(state, 1);
}

unsigned printCmd58(struct scriptstate *state) {
	return printCmdGeneric(state, 0);
}

unsigned printCmd59(struct scriptstate *state) {
	return printCmdGeneric(state, 1);
}

unsigned printCmd5A(struct scriptstate *state) {
	return printCmdGeneric(state, 1);
}

unsigned printCmd5B(struct scriptstate *state) {
	return printCmdGeneric(state, 2);
}

unsigned printCmd5C(struct scriptstate *state) { /* 3 */
	return printCmdGeneric(state, 0);
}

unsigned printCmd5D(struct scriptstate *state) { /* 0 */
	return printCmdGeneric(state, 1);
}

unsigned printCmd5E(struct scriptstate *state) { /* 0 */
	return printCmdGeneric(state, 1);
}

unsigned printCmd5F(struct scriptstate *state) {
	return printCmdGeneric(state, 3);
}

/* commands introduced after GS1 gba */

unsigned printCmd60(struct scriptstate *state) { /* 4 */
	return printCmdGeneric(state, 0);
}

unsigned printCmd61(struct scriptstate *state) {
	return printCmdGeneric(state, 3);
}

unsigned printCmd62(struct scriptstate *state) {
	return printCmdGeneric(state, 0);
}

unsigned printCmd63(struct scriptstate *state) {
	return printCmdGeneric(state, 0);
}

unsigned printCmd64(struct scriptstate *state) {
	return printCmdGeneric(state, 1);
}

unsigned printCmd65(struct scriptstate *state) {
	return printCmdGeneric(state, 2);
}

unsigned printCmd66(struct scriptstate *state) { /* 3 */
	return printCmdGeneric(state, 2);
}

unsigned printCmd67(struct scriptstate *state) {
	return printCmdGeneric(state, 0);
}

unsigned printCmd68(struct scriptstate *state) {
	return printCmdGeneric(state, 0);
}

unsigned printCmd69(struct scriptstate *state) { /* 4 */
	return printCmdGeneric(state, 2);
}

unsigned printCmd6A(struct scriptstate *state) {
	return printCmdGeneric(state, 1);
}

unsigned printCmd6B(struct scriptstate *state) {
	return printCmdGeneric(state, 3);
}

unsigned printCmd6C(struct scriptstate *state) { /* 0 */
	return printCmdGeneric(state, 1);
}

unsigned printCmd6D(struct scriptstate *state) {
	return printCmdGeneric(state, 1);
}

unsigned printCmd6E(struct scriptstate *state) {
	return printCmdGeneric(state, 1);
}

unsigned printCmd6F(struct scriptstate *state) {
	return printCmdGeneric(state, 1);
}

unsigned printCmd70(struct scriptstate *state) {
	return printCmdGeneric(state, 3);
}

unsigned printCmd71(struct scriptstate *state) {
	return printCmdGeneric(state, 3);
}

unsigned printCmd72(struct scriptstate *state) {
	return printCmdGeneric(state, 0);
}

unsigned printCmd73(struct scriptstate *state) {
	return printCmdGeneric(state, 0);
}

unsigned printCmd74(struct scriptstate *state) {
	/* this appears to be, among other things, related to minigames (vase and luminol) as well as the staff roll/credits
           it appears a lot more often though (switching touchsceen display modes?) */
	
	return printCmdGeneric(state, 2);
}

unsigned printCmd75(struct scriptstate *state) {
	return printCmdGeneric(state, 4);
}

unsigned printCmd76(struct scriptstate *state) { /* 2 */
	return printCmdGeneric(state, 0);
}

unsigned printCmd77(struct scriptstate *state) { /* 2 */
	return printCmdGeneric(state, 0);
}

unsigned printCmd78(struct scriptstate *state) {
	return printCmd36(state);
}

unsigned printCmd79(struct scriptstate *state) {
	return printCmdGeneric(state, 0);
}

unsigned printCmd7A(struct scriptstate *state) { /* 1 */
	return printCmdGeneric(state, 0);
}

unsigned printCmd7B(struct scriptstate *state) { /* 2 */
	return printCmdGeneric(state, 0);
}

unsigned printCmd7C(struct scriptstate *state) {
	return printCmdGeneric(state, 0);
}

unsigned printCmd7D(struct scriptstate *state) { /* 1 */
	return printCmdGeneric(state, 0);
}

unsigned printCmd7E(struct scriptstate *state) { /* 1 */
	return printCmdGeneric(state, 0);
}

unsigned printCmd7F(struct scriptstate *state) { /* 1 */
	return printCmdGeneric(state, 0);
}

unsigned printCmd80(struct scriptstate *state) {
	return printCmdGeneric(state, 0);
}

unsigned printCmd81(struct scriptstate *state) {
	return printCmdGeneric(state, 0);
}

unsigned printCmd82(struct scriptstate *state) {
	return printCmdGeneric(state, 0);
}

unsigned printCmd83(struct scriptstate *state) {
	return printCmdGeneric(state, 0);
}

unsigned printCmd84(struct scriptstate *state) {
	return printCmdGeneric(state, 0);
}

unsigned printCmd85(struct scriptstate *state) {
	return printCmdGeneric(state, 0);
}

unsigned printCmd86(struct scriptstate *state) {
	return printCmdGeneric(state, 0);
}

unsigned printCmd87(struct scriptstate *state) {
	return printCmdGeneric(state, 0);
}

unsigned printCmd88(struct scriptstate *state) {
	return printCmdGeneric(state, 0);
}

unsigned printCmd89(struct scriptstate *state) {
	return printCmdGeneric(state, 0);
}

unsigned printCmd8A(struct scriptstate *state) {
	return printCmdGeneric(state, 0);
}

unsigned printCmd8B(struct scriptstate *state) {
	return printCmdGeneric(state, 0);
}

unsigned printCmd8C(struct scriptstate *state) {
	return printCmdGeneric(state, 0);
}

unsigned printCmd8D(struct scriptstate *state) {
	return printCmdGeneric(state, 0);
}

unsigned printCmd8E(struct scriptstate *state) {
	return printCmdGeneric(state, 0);
}

unsigned printCmd8F(struct scriptstate *state) {
	return printCmdGeneric(state, 0);
}
command commands[144] = {
	{ printCmd00, "setup_section" }, 		/* does something? */
	{ printCmd01, "linebreak" }, 			/* linebreak */
	{ printCmd02, "pagebreak" }, 			/* paragraph, ends current textbox, waits for player interaction */
	{ printCmd03, "textcolor" }, 			/* text color, args: 0 white, 1 red, 2 blue, 3 green */
	{ printCmd04, "pause" },			/* pause the game, waits for player interaction */
	{ printCmd05, "music" }, 			/* change the music, args sequence(SDAT index) - fadein time frames (on resume only?) */
	{ printCmd06, "sound" }, 			/* play a sound effect, args ? - ? */
	{ printCmd07, "fullscreen_text" }, 		/* switches to fullscreen display, GBA only? */
	{ printCmd08, "finger_choice_2_args_jmp" }, 	/* select between two choices (either in FS mode with previous opcode or from touchscreen), args: pointers to respective choices in script, followed by endjump? */
	{ printCmd09, "finger_choice_3_args_jmp" }, 	/* see above with 3 choices */
	{ printCmd0A, "rejmp" }, 			/* pointer to jump to for multiple choice questions failed once */
	{ printCmd0B, "speed" }, 			/* change text speed, args: frames/character */
	{ printCmd0C, "wait" }, 			/* wait for specified time units, args: frames to wait */
	{ printCmd0D, "endjmp" }, 			/* terminates a jump, usually found after all other jumpstatements */
	{ printCmd0E, "name" }, 			/* change the name in the top left of a textbox, apparently, arg needs to be shifted to the right by 8 (actual value in upper 8 bits of 16bit arg) */
	{ printCmd0F, "testimony_box" }, 		/* begins a testimony section, args: ? - ? */
	{ printCmd10, "flagctl"  },			/* modifies flags (set/unset) */
	{ printCmd11, "evidence_window_plain" }, 	/* show evidence window without lifebar (as when pressing R) */
	{ printCmd12, "bgcolor" }, 			/* flashes the screen? args(?) color, fadein, fadeout in frames? args in python not clear enough :/ */
	{ printCmd13, "showevidence" }, 		/* displays little evidence box with SFX, args: evidence to show? */
	{ printCmd14, "removeevidence" }, 		/* removes above box with SFX */
	{ printCmd15, "special_jmp" }, 			/* used at the end of testimony boxes for saves and resets? */
	{ printCmd16, "savegame" }, 			/* displays saving screen */
	{ printCmd17, "newevidence" }, 			/* adds evidence to court record with animation and SFX, args: object to add */
	{ printCmd18, "newevidence_noanim" }, 		/* plays new evidence sound with nothing else?, args: ? */
	{ printCmd19, "cmd19" }, 			/* ? */
	{ printCmd1A, "swoosh" }, 			/* starts panning the camera from one side of court to the other (always followed by wait 1e?), args: ? */
	{ printCmd1B, "bg" }, 				/* change background image, args: background to display */
	{ printCmd1C, "hidetextbox" }, 			/* show textbox(border?), args: 0 show, 1 hide */
	{ printCmd1D, "shift_background" }, 		/* shifts background?, args: x*256 for direction + pixels/frame */
	{ printCmd1E, "person" }, 			/* change the character image shown on-screen, args: ?,?,? */
	{ printCmd1F, "hideperson" }, 			/* instantly hide the character image */
	{ printCmd20, "cmd20" }, 			/* ? */
	{ printCmd21, "evidence_window_lifebar" }, 	/* show evidence window with lifebar */
	{ printCmd22, "fademusic" }, 			/* fades music, args: fadeout/in, frames until 0/full volume */
	{ printCmd23, "cmd23" }, 			/* ? */
	{ printCmd24, "reset" }, 			/* reset the game to title screen */
	{ printCmd25, "cmd25" }, 			/* ? */
	{ printCmd26, "hide_court_record_button" }, 	/* shows court-record button, args: 0 show, 1 hide */
	{ printCmd27, "shake" }, 			/* shakes the screen?, args: ? - ? (first seems to be 1e only and second is changing?) */
	{ printCmd28, "testemony_animation" }, 		/* display "testimony" animation shown before witness testifies, args: ? */
	{ printCmd29, "return_to_testimony" }, 		/* returns from a wrong answer to the point in testimony, where objection was called?, args: ? */
	{ printCmd2A, "cmd2A" },  			/* ?, always followed by endjmp? */
	{ printCmd2B, "cmd2B" },  			/* ? */
	{ printCmd2C, "jmp" }, 				/* jumps to pointer, args: pointer to target */
	{ printCmd2D, "nextpage_button" }, 		/* same as p */
	{ printCmd2E, "nextpage_nobutton" }, 		/* paragraph, ends textbox, no interaction */
	{ printCmd2F, "animation" }, 			/* display animation (such as "objection!"), args: ? */
	{ printCmd30, "cmd30" },  			/* ? */
	{ printCmd31, "personvanish" }, 		/* makes characer vanish, args: ? */
	{ printCmd32, "cmd32" },  			/* ? */
	{ printCmd33, "setmovelocations" },		/* sets locations available to move to */
	{ printCmd34, "fadetoblack" }, 			/* fades to black, args: ? */
	{ printCmd35, "cmd35" },  			/* ? */
	{ printCmd36, "cmd36" },			/* ?, unclear description in python */
	{ printCmd37, "cmd37" },  			/* ? */
	{ printCmd38, "cmd38" },  			/* ? */
	{ printCmd39, "littlesprite" }, 		/* makes blip for characters on map in case 4 appear?, args: blip to show? */
	{ printCmd3A, "cmd3A" },			/* ? */
	{ printCmd3B, "cmd3B" },  			/* Animation related (begin animation?) */
	{ printCmd3C, "cmd3C" },  			/* Animation related (makes argument blip flash?) */
	{ printCmd3D, "cmd3D" },  			/* Animation related (stop animation?) */
	{ printCmd3E, "cmd3E" },  			/* ? */
	{ printCmd3F, "cmd3F" },  			/* unknown jump, args: ? */
	{ printCmd40, "cmd40" },  			/* ? */
	{ printCmd41, "cmd41" },  			/* ? */
	{ printCmd42, "togglevoicesfx" }, 		/* toggles play of voice/typewriter sfx, args: 0 play - 1 stop */
	{ printCmd43, "show_lifebar" }, 		/* show lifebar (with animation), args: 0 slide out - 1 slide in? */
	{ printCmd44, "guilty" }, 			/* play guilty animation, args: ? */
	{ printCmd45, "cmd45" },  			/* jump at the end of special messages? */
	{ printCmd46, "bgtile" }, 			/* change all background tiles, args: tile to change to */
	{ printCmd47, "cmd47" },  			/* ? */
	{ printCmd48, "cmd48" },  			/* ? */
	{ printCmd49, "wingame" }, 			/* return to title, unlock all cases */
	{ printCmd4A, "cmd4A" },			/* ?, crash? */
	{ printCmd4B, "cmd4B" },  			/* ? */
	{ printCmd4C, "cmd4C" },  			/* ? */
	{ printCmd4D, "cmd4D" },  			/* ?, not tested? */
	{ printCmd4E, "wait_noanim" }, 			/* wait for specified time, no character animation, args: time to wait */
	{ printCmd4F, "cmd4F" },  			/* ? */
	{ printCmd50, "cmd50" },  			/* ? */
	{ printCmd51, "cmd51" },  			/* ? */
	{ printCmd52, "cmd52" },  			/* ? */
	{ printCmd53, "cmd53" },  			/* ? */
	{ printCmd54, "lifebarset" }, 			/* various lifebar related things?, args: ?, ? */
	{ printCmd55, "cmd55" },			/* ?, crash? */
	{ printCmd56, "cmd56" },  			/* ? */
	{ printCmd57, "psychoblock" }, 			/* play psychoblock chain and lock appearance animation, args: locks to show */
	{ printCmd58, "cmd58" },  			/* ? */
	{ printCmd59, "cmd59" },  			/* ? */
	{ printCmd5A, "cmd5A" },  			/* ? */
	{ printCmd5B, "cmd5B" },  			/* ? */
	{ printCmd5C, "cmd5C" },			/* ?, crash? */
	{ printCmd5D, "toggle_center_text" },		/* toggles text centering, args: 0 normal alignment - 1 centered */
	{ printCmd5E, "cmd5E" },			/* ? */
	{ printCmd5F, "cmd5F" },  			/* ? */
	/* commands introduced after GS1 gba */
	{ printCmd60, "cmd60" },			/* ?, crash? */
	{ printCmd61, "cmd61" },  			/* ? */
	{ printCmd62, "cmd62" },  			/* ? */
	{ printCmd63, "cmd63" },  			/* ?, crash? */
	{ printCmd64, "cmd64" },  			/* show special effect?, args: ? */
	{ printCmd65, "cmd65" },  			/* ? */
	{ printCmd66, "cmd66" },			/* ? */
	{ printCmd67, "cmd67" },  			/* ? */
	{ printCmd68, "cmd68" },  			/* ? */
	{ printCmd69, "bganim" },			/* play fullscreen animation, args: ? */
	{ printCmd6A, "switchscript" }, 		/* loads new script and jumps to beginning, args: current case? (python is unclear) */
	{ printCmd6B, "cmd6B" },  			/* Animation related (load animation? used for maps?) */
	{ printCmd6C, "cmd6C" },			/* ? */
	{ printCmd6D, "cmd6D" },  			/* ? */
	{ printCmd6E, "cmd6E" },  			/* ? */
	{ printCmd6F, "cmd6F" },  			/* ? */
	{ printCmd70, "cmd70" },  			/* ? */
	{ printCmd71, "cmd71" },  			/* ? */
	{ printCmd72, "cmd72" },  			/* ? */
	{ printCmd73, "cmd73" },  			/* ?, crash? */
	{ printCmd74, "cmd74" },			/* ?, crash? */
	{ printCmd75, "cmd75" },  			/* ?, crash? */
	{ printCmd76, "cmd76" },			/* ?, crash? */
	{ printCmd77, "cmd77" },			/* ?, crash? */
	/* unity maps cmd78 to cmd36. possibly something to do with localization? */
	{ printCmd78, "cmd78" },			/* ?, crash? */
	{ printCmd79, "cmd79" },  			/* ?, crash? */
	{ printCmd7A, "cmd7A" },			/* reset to capcom animation? */
	{ printCmd7B, "cmd7B" },			/* ?, crash? */
	{ printCmd7C, "cmd7C" },  			/* ?, crash? */
	{ printCmd7D, "cmd7D" },			/* reset to capcom animation? */
	{ printCmd7E, "cmd7E" },			/* ?, crash? */
	{ printCmd7F, "cmd7F" }, 			/* ?, crash? */
	{ printCmd80, "cmd80" },  			/* dummy for apollotesting */
	{ printCmd81, "cmd81" },  			/* dummy for apollotesting */
	{ printCmd82, "cmd82" },  			/* dummy for apollotesting */
	{ printCmd83, "cmd83" },  			/* dummy for apollotesting */
	{ printCmd84, "cmd84" },  			/* dummy for apollotesting */
	{ printCmd85, "cmd85" },  			/* dummy for apollotesting */
	{ printCmd86, "cmd86" },  			/* dummy for apollotesting */
	{ printCmd87, "cmd87" },  			/* dummy for apollotesting */
	{ printCmd88, "cmd88" },  			/* dummy for apollotesting */
	{ printCmd89, "cmd89" },  			/* dummy for apollotesting */
	{ printCmd8A, "cmd8A" },  			/* dummy for apollotesting */
	{ printCmd8B, "cmd8B" },  			/* dummy for apollotesting */
	{ printCmd8C, "cmd8C" },  			/* dummy for apollotesting */
	{ printCmd8D, "cmd8D" },  			/* dummy for apollotesting */
	{ printCmd8E, "cmd8E" },  			/* dummy for apollotesting */
	{ printCmd8F, "cmd8F" }				/* dummy for apollotesting */
};

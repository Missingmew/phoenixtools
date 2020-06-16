#include <stdio.h>

#include "phoenixscript_commands.h"
#include "phoenixscript_data.h"


/* structure and prettyprinting functions for commands as presented in the GBA/NDS games */

/* personvanish arg1:
	03 - show
	04 - hide
*/
/* numbers in comments are arguments as stated in MessageSystem from unity */

int isSectionStart(uint32_t *list, unsigned count, unsigned index) {
	for(unsigned i = 0; i < count; i++) if(list[i] == index) return i;
	return -1;
}

int isLabelLocation(jumplutpack *lut, unsigned count, unsigned section, unsigned offset) {
	for(unsigned i = 0; i < count; i++) if(section == lut[i].section && offset == lut[i].offset/2) return i;
	return -1;
}

int isLocalLabelLocation(struct localjumpinfo *jumps, unsigned count, unsigned section, unsigned offset) {
	for(unsigned i = 0; i < count; i++) if(section == jumps[i].index && offset == jumps[i].target) return 1;
	return -1;
}

unsigned printCmdGeneric(struct scriptstate *state, unsigned args) {
	unsigned i;
	if(state->outputenabled) {
		state->textidx += sprintf(state->textfile+state->textidx, "%s", commandnames[state->script[state->scriptidx]]);
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
		unsigned color = state->script[state->scriptidx+1];
		if(color < sizeofarr(colors)) {
			state->textidx += sprintf(state->textfile+state->textidx, "%s %s\n", commandnames[state->script[state->scriptidx]], colors[color]);
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
		unsigned musicid = state->script[state->scriptidx+1];
		unsigned fadetime = state->script[state->scriptidx+2];
		if(isvalidarrentry(musicid, sound_data[ARRGAMENUM(state->gamenum)])) {
			state->textidx += sprintf(state->textfile+state->textidx, "%s %s, %u\n", commandnames[state->script[state->scriptidx]], sound_data[ARRGAMENUM(state->gamenum)][musicid], fadetime);
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
			unsigned seNum = state->script[state->scriptidx+1] >> 8;
			unsigned stopplay = state->script[state->scriptidx+1] & 1;
			if(isvalidarrentry(seNum, sound_data[ARRGAMENUM(state->gamenum)])) {
				state->textidx += sprintf(state->textfile+state->textidx, "%s %s, %s\n", commandnames[state->script[state->scriptidx]], sound_data[ARRGAMENUM(state->gamenum)][seNum], soundplay[stopplay]);
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
		unsigned option1section = state->script[state->scriptidx+1]-128;
		unsigned option2section = state->script[state->scriptidx+2]-128;
		state->textidx += sprintf(state->textfile+state->textidx, "%s %u, %u\n", commandnames[state->script[state->scriptidx]], option1section, option2section);
		state->scriptidx += 1+2;
	}
	return 2;
}

unsigned printCmd09(struct scriptstate *state) {
	if(state->outputenabled) {
		unsigned option1section = state->script[state->scriptidx+1]-128;
		unsigned option2section = state->script[state->scriptidx+2]-128;
		unsigned option3section = state->script[state->scriptidx+3]-128;
		state->textidx += sprintf(state->textfile+state->textidx, "%s %u, %u, %u\n", commandnames[state->script[state->scriptidx]], option1section, option2section, option3section);
		state->scriptidx += 1+3;
	}
	return 3;
}

unsigned printCmd0A(struct scriptstate *state) {
	if(state->outputenabled) {
		unsigned targetsection = state->script[state->scriptidx+1]-128;
		state->textidx += sprintf(state->textfile+state->textidx, "%s %u\n", commandnames[state->script[state->scriptidx]], targetsection);
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
		unsigned nameid = (state->script[state->scriptidx+1] >> 8);
		unsigned whichside = (state->script[state->scriptidx+1] & 0xF);
		if(isvalidarrentry(nameid, speakers[ARRGAMENUM(state->gamenum)]) && whichside < sizeofarr(showside)) {
			state->textidx += sprintf(state->textfile+state->textidx, "%s %s, %s\n", commandnames[state->script[state->scriptidx]], speakers[ARRGAMENUM(state->gamenum)][nameid], showside[whichside] );
			state->scriptidx += 1+1;
		}
		else return printCmdGeneric(state, 1);
	}
	return 1;
}

unsigned printCmd0F(struct scriptstate *state) {
	if(state->outputenabled) {
		/* this does _NOT_ use the 128 offset for the section */
		unsigned presssection = state->script[state->scriptidx+1];
		unsigned hidetextbox = state->script[state->scriptidx+2];
		if(hidetextbox < sizeofarr(testimonypress)) {
			state->textidx += sprintf(state->textfile+state->textidx, "%s %u, %s\n", commandnames[state->script[state->scriptidx]], presssection, testimonypress[hidetextbox]);
			state->scriptidx += 1+2;
		}
		else return printCmdGeneric(state, 2);
	}
	return 1;
}

unsigned printCmd10(struct scriptstate *state) {
	if(state->outputenabled) {
		unsigned id = state->script[state->scriptidx+1] & 0xFF;
		unsigned type = (state->script[state->scriptidx+1] & 0x7F00) >> 8; // needs more checking... maybe a bank or something?
		unsigned set = state->script[state->scriptidx+1] >> 15;
		state->textidx += sprintf(state->textfile+state->textidx, "%s %u, %u, %u\n", commandnames[state->script[state->scriptidx]], type, id, set);
		state->scriptidx += 1+1;
	}
	return 1;
}

unsigned printCmd11(struct scriptstate *state) {
	return printCmdGeneric(state, 0);
}

unsigned printCmd12(struct scriptstate *state) {
	if(state->outputenabled) {
		unsigned delay = state->script[state->scriptidx+1] & 0xFF;
		unsigned mode = state->script[state->scriptidx+1] >> 8;
		unsigned delta = state->script[state->scriptidx+2]; // amount of blending to do per frame
		unsigned target = state->script[state->scriptidx+3];
		if(mode < sizeofarr(fademode)) {
			state->textidx += sprintf(state->textfile+state->textidx, "%s %u, %s, %u, %u\n", commandnames[state->script[state->scriptidx]], delay, fademode[mode], delta, target);
			state->scriptidx += 1+3;
		}
		else return printCmdGeneric(state, 3);
	}
	return 3;
}

unsigned printCmd13(struct scriptstate *state) {
	if(state->outputenabled) {
		unsigned evidenceid = state->script[state->scriptidx+1] & 0xFF;
		unsigned whichside = state->script[state->scriptidx+1] >> 8;
		if(whichside < sizeofarr(showside)) {
			state->textidx += sprintf(state->textfile+state->textidx, "%s %u, %s\n", commandnames[state->script[state->scriptidx]], evidenceid, showside[whichside]);
			state->scriptidx += 1+1;
		}
		else return printCmdGeneric(state, 1);
	}
	return 1;
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
		unsigned bgid = state->script[state->scriptidx+1] & 0x7FFF;
		unsigned shift = state->script[state->scriptidx+1] >> 15;
		//~ if(bgid < sizeofarr(backgrounds[ARRGAMENUM(state->gamenum)]) && backgrounds[ARRGAMENUM(state->gamenum)][bgid]) {
		if(isvalidarrentry(bgid, backgrounds[ARRGAMENUM(state->gamenum)])) {
			state->textidx += sprintf(state->textfile+state->textidx, "%s %s, %s\n", commandnames[state->script[state->scriptidx]], backgrounds[ARRGAMENUM(state->gamenum)][bgid], bgshift[shift] );
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
		unsigned direction = state->script[state->scriptidx+1] >> 8;
		unsigned speed = state->script[state->scriptidx+1] & 0xFF;
		state->textidx += sprintf(state->textfile+state->textidx, "%s %s, %u\n", commandnames[state->script[state->scriptidx]], shiftdirection[direction], speed);
		state->scriptidx += 1+1;
	}
	return 1;
}

unsigned printCmd1E(struct scriptstate *state) {
	if(state->outputenabled) {
		unsigned personid = state->script[state->scriptidx+1] & 0x1FFF;
		unsigned hflip = (state->script[state->scriptidx+1] >> 13) & 1;
		unsigned placement = state->script[state->scriptidx+1] >> 14;
		unsigned talkinganimation = state->script[state->scriptidx+2];
		unsigned idleanimation = state->script[state->scriptidx+3];
		//~ if(ISNDS(state->gamenum) && personid < sizeofarr(speakers[ARRGAMENUM(state->gamenum)]) && speakers[ARRGAMENUM(state->gamenum)][personid]) {
		if(ISNDS(state->gamenum) && isvalidarrentry(personid, speakers[ARRGAMENUM(state->gamenum)]) && isvalidarrentry(talkinganimation, personanimations[state->gamenum]) && isvalidarrentry(idleanimation, personanimations[state->gamenum])) {
			state->textidx += sprintf(state->textfile+state->textidx, "%s %s, %s, %u, %s, %s\n", commandnames[state->script[state->scriptidx]], speakers[ARRGAMENUM(state->gamenum)][personid], personplacement[placement], hflip, personanimations[state->gamenum][talkinganimation], personanimations[state->gamenum][idleanimation]);
			state->scriptidx += 1+3;
		}
		/* GBA uses hard offsets into data instead of indices */
		else if(ISGBA(state->gamenum) && isvalidarrentry(personid, speakers[ARRGAMENUM(state->gamenum)])) {
			state->textidx += sprintf(state->textfile+state->textidx, "%s %s, %s, %u, %X, %X\n", commandnames[state->script[state->scriptidx]], speakers[ARRGAMENUM(state->gamenum)][personid], personplacement[placement], hflip, talkinganimation, idleanimation);
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
	/* first arg is skipped by the game in GS1GBA, not sure about others and NDS */
	if(state->outputenabled) {
		unsigned arg1 = state->script[state->scriptidx+1];
		unsigned fadetime = state->script[state->scriptidx+2];
		state->textidx += sprintf(state->textfile+state->textidx, "%s %u, %u\n", commandnames[state->script[state->scriptidx]], arg1, fadetime);
		state->scriptidx += 1+2;
	}
	return 2;
}

unsigned printCmd23(struct scriptstate *state) {
	/* first arg is skipped by the game in GS1GBA, not sure about others and NDS */
	if(state->outputenabled) {
		unsigned arg1 = state->script[state->scriptidx+1];
		unsigned pause = state->script[state->scriptidx+2];
		if(pause < sizeofarr(musicpause)) {
			state->textidx += sprintf(state->textfile+state->textidx, "%s %u, %s\n", commandnames[state->script[state->scriptidx]], arg1, musicpause[pause]);
			state->scriptidx += 1+2;
		}
		else return printCmdGeneric(state, 2);
	}
	return 2;
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
		unsigned unk1 = state->script[state->scriptidx+1];
		unsigned anistate = state->script[state->scriptidx+2];
		if(anistate < sizeofarr(animationstate)) {
			state->textidx += sprintf(state->textfile+state->textidx, "%s %u, %s\n", commandnames[state->script[state->scriptidx]], unk1, animationstate[anistate]);
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
		state->textidx += sprintf(state->textfile+state->textidx, "%s", commandnames[state->script[state->scriptidx]]);
		
		/* this block just iterates through all arguments, sanity checks them and then either prints the corresponding string or a number */
		/* extra checking done only for the first argument to get the output text done right */
		if(isvalidarrentry(state->script[state->scriptidx+1], locations[ARRGAMENUM(state->gamenum)])) {
			state->textidx += sprintf(state->textfile+state->textidx, " %s", locations[ARRGAMENUM(state->gamenum)][state->script[state->scriptidx+1]]);
		}
		else state->textidx += sprintf(state->textfile+state->textidx, " %u", state->script[state->scriptidx+1]);
		
		for(i = 2; i < 6; i++) {
			if(isvalidarrentry(state->script[state->scriptidx+i], locations[ARRGAMENUM(state->gamenum)])) {
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
	if(state->outputenabled) {
		unsigned flaghint = state->script[state->scriptidx+1] & 1;
		unsigned whichflag = state->script[state->scriptidx+1] >> 8;
		unsigned jumphint = !!(state->script[state->scriptidx+1] & 0x80);
		/* depending on jumphint this can be either an index into the script special data or a straight offset */
		unsigned spcidx_thisoffset = state->script[state->scriptidx+2];
		unsigned offset, targetsection;
		
		if(jumphint) {
			unsigned specialindex = spcidx_thisoffset - state->numsections;
			offset = state->jumplut[specialindex].offset / 2;
			targetsection = state->jumplut[specialindex].section;
			//~ fprintf(stderr, "%s at %08x in section %u is farjump to %u + %u using specialindex %08x\n", commandnames[state->script[state->scriptidx]], state->scriptidx, state->section, targetsection, offset, specialindex);
			state->textidx += sprintf(state->textfile+state->textidx, "%s %s, %u, %s, label%u_%u\n", commandnames[state->script[state->scriptidx]], cmd35flaghint[flaghint], whichflag, cmd35jumphint[jumphint], targetsection, offset);
		}
		else {
			unsigned hasglobal;
			offset = spcidx_thisoffset/2;
			hasglobal = -1 < isLabelLocation(state->jumplut, state->numjumplut, state->section, offset); 
			//~ fprintf(stderr, "%s at %08x in section %u is nearjump to %u\n", commandnames[state->script[state->scriptidx]], state->scriptidx, state->section, offset);
			state->textidx += sprintf(state->textfile+state->textidx, "%s %s, %u, %s, %slabel%u_%u\n", commandnames[state->script[state->scriptidx]], cmd35flaghint[flaghint], whichflag, cmd35jumphint[jumphint], hasglobal ? "" : ".", state->section, offset);
		}
		//~ state->textidx += sprintf(state->textfile+state->textidx, "%s %s, %u, %u + %u\n", commandnames[state->script[state->scriptidx]], cmd35flaghint[flaghint], whichflag, targetsection, offset);
		state->scriptidx += 1+2;
	}
	return 2;
}

unsigned printCmd36(struct scriptstate *state) {
	/* apollo currently errors out for this, probably related to other commands... */
	if(state->gamenum == GAME_APOLLO) {
		return printCmdGeneric(state, 1);
	}
	else {
		if(state->outputenabled) {
			unsigned specialindex = state->script[state->scriptidx+1] - state->numsections;
			unsigned offset = state->jumplut[specialindex].offset / 2;
			unsigned targetsection = state->jumplut[specialindex].section;
			//~ fprintf(stderr, "%s at %08x in section %u is farjump to %u + %u using specialindex %08x\n", commandnames[state->script[state->scriptidx]], state->scriptidx, state->section, targetsection, offset, specialindex);
			
			//~ state->textidx += sprintf(state->textfile+state->textidx, "%s %u + %u\n", commandnames[state->script[state->scriptidx]], targetsection, offset);
			state->textidx += sprintf(state->textfile+state->textidx, "%s label%u_%u\n", commandnames[state->script[state->scriptidx]], targetsection, offset);
			state->scriptidx += 1+1;
		}
		return 1;
	}
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

unsigned printCmd3A(struct scriptstate *state) {
	/* has 2 args in GS1GBA */
	if(state->gamenum == GAME_GS1GBA) {
		return printCmdGeneric(state, 2);
	}
	else return printCmdGeneric(state, 3);
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

unsigned printCmd4A(struct scriptstate *state) {
	return printCmdGeneric(state, 1);
}

unsigned printCmd4B(struct scriptstate *state) {
	return printCmdGeneric(state, 1);
}

unsigned printCmd4C(struct scriptstate *state) {
	return printCmdGeneric(state, 0);
}

/* all commands below are nullsubs in GS1 gba */

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

unsigned printCmd5C(struct scriptstate *state) {
	return printCmdGeneric(state, 3);
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

unsigned printCmd60(struct scriptstate *state) {
	if(state->outputenabled) {
		unsigned unk1 = state->script[state->scriptidx+1];
		unsigned itemid = state->script[state->scriptidx+2];
		unsigned correctsection = state->script[state->scriptidx+3]-128;
		unsigned othersection = state->script[state->scriptidx+4]-128;
		
		state->textidx += sprintf(state->textfile+state->textidx, "%s %u, %u, %u, %u\n", commandnames[state->script[state->scriptidx]], unk1, itemid, correctsection, othersection);
		state->scriptidx += 1+4;
	}
	return 4;
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

unsigned printCmd66(struct scriptstate *state) {
	return printCmdGeneric(state, 3);
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

unsigned printCmd76(struct scriptstate *state) {
	return printCmdGeneric(state, 2);
}

unsigned printCmd77(struct scriptstate *state) {
	return printCmdGeneric(state, 2);
}

unsigned printCmd78(struct scriptstate *state) {
	return printCmd36(state);
}

unsigned printCmd79(struct scriptstate *state) {
	return printCmdGeneric(state, 0);
}

unsigned printCmd7A(struct scriptstate *state) {
	return printCmd36(state);
}

unsigned printCmd7B(struct scriptstate *state) {
	return printCmdGeneric(state, 2);
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

/* all of the following commands were added to support apollo */

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

/* names for the commands, eight names per line */
/* length of 256 is arbitrarily chosen but should be enough to hold all possible strings
   its required to allow the definition of commands below to use commandnames for initialization */
char *commandnames[144] = {
/* 00h */ "section_setup", "linebreak", "pagebreak", "textcolor", "pause", "music", "sound", "fullscreen_text",
/* 08h */ "finger_choice_2_args_jmp", "finger_choice_3_args_jmp", "pagebreak_section", "speed", "wait", "section_end", "name", "testimony_box",
/* 10h */ "flagctl", "evidence_window_plain", "screen_fade", "showevidence", "removeevidence", "halt", "scenario_end_save", "newevidence",
/* 18h */ "newevidence_noanim", "cmd19", "swoosh", "bg", "hidetextbox", "shift_background", "person", "hideperson",
/* 20h */ "cmd20", "evidence_window_lifebar", "fademusic", "pausemusic", "reset", "cmd25", "hide_court_record_button", "shake",
/* 28h */ "testemony_animation", "return_to_testimony", "cmd2A", "cmd2B", "jmp", "nextpage_button", "nextpage_nobutton", "animation",
/* 30h */ "cmd30", "personvanish", "cmd32", "setmovelocations", "fadetoblack", "jmp_conditional", "jmp_far", "cmd37",
/* 38h */ "cmd38", "littlesprite", "cmd3A", "cmd3B", "cmd3C", "cmd3D", "cmd3E", "cmd3F",
/* 40h */ "cmd40", "cmd41", "togglevoicesfx", "show_lifebar", "guilty", "cmd45", "bgtile", "cmd47",
/* 48h */ "cmd48", "wingame", "cmd4A", "cmd4B", "cmd4C",
	
/* commands following are nullsubs in GS1 gba */
/* 4Dh */ "cmd4D", "wait_noanim", "cmd4F",
/* 50h */ "cmd50", "cmd51", "cmd52", "cmd53", "lifebarset", "cmd55", "cmd56", "psychoblock",
/* 58h */ "cmd58", "cmd59", "cmd5A", "cmd5B", "cmd5C", "toggle_center_text", "cmd5E", "cmd5F",
	
/* commands introduced after GS1 gba */
/* 60h */ "psychelock_itemchallenge", "cmd61", "cmd62", "cmd63", "cmd64", "cmd65", "cmd66", "cmd67",
/* 68h */ "cmd68", "bganim", "switchscript", "cmd6B", "cmd6C", "cmd6D", "cmd6E", "cmd6F",
/* 70h */ "cmd70", "cmd71", "cmd72", "cmd73", "cmd74", "cmd75", "cmd76", "cmd77",

/* unity maps cmd78 to cmd36. possibly something to do with localization? */
/* 78h */ "cmd78", "cmd79", "jmp_far_nolife", "cmd7B", "cmd7C", "cmd7D", "cmd7E", "cmd7F",

/* all of the following commands were added to support apollo */
/* 80h */ "cmd80", "cmd81", "cmd82", "cmd83", "cmd84", "cmd85", "cmd86", "cmd87",
/* 88h */ "cmd88", "cmd89", "cmd8A", "cmd8B", "cmd8C", "cmd8D", "cmd8E", "cmd8F"
};

/* note that the documentation here is a horrible mix of leftovers from PWSE,
   half baked reversing attempts and extra knowledge from GS1GBA.
   take it with a grain of salt */
unsigned (*printcommands[0x90])(struct scriptstate *) = {
	printCmd00,	/* does something? */
	printCmd01,	/* linebreak */
	printCmd02,	/* paragraph, ends current textbox, waits for player interaction */
	printCmd03,	/* text color, args: 0 white, 1 red, 2 blue, 3 green */
	printCmd04,	/* pause the game, waits for player interaction */
	printCmd05,	/* change the music, args sequence(SDAT index) - fadein time frames (on resume only?) */
	printCmd06,	/* play a sound effect, args ? - ? */
	printCmd07,	/* switches to fullscreen display, GBA only? */
	printCmd08,	/* select between two choices (either in FS mode with previous opcode or from touchscreen), args: pointers to respective choices in script, followed by endjump? */
	printCmd09,	/* see above with 3 choices */
	printCmd0A,	/* pointer to jump to for multiple choice questions failed once */
	printCmd0B,	/* change text speed, args: frames/character */
	printCmd0C,	/* wait for specified time units, args: frames to wait */
	printCmd0D,	/* terminates a jump, usually found after all other jumpstatements */
	printCmd0E,	/* change the name in the top left of a textbox, apparently, arg needs to be shifted to the right by 8 (actual value in upper 8 bits of 16bit arg) */
	printCmd0F,	/* begins a testimony section, args: ? - ? */
	printCmd10,	/* modifies flags (set/unset) */
	printCmd11,	/* show evidence window without lifebar (as when pressing R) */
	printCmd12,	/* flashes the screen? args(?) color, fadein, fadeout in frames? args in python not clear enough :/ */
	printCmd13,	/* displays little evidence box with SFX, args: evidence to show? */
	printCmd14,	/* removes above box with SFX */
	printCmd15,	/* halts script execution (repeats this command). script pointer will be changed externally (by player interaction) */
	printCmd16,	/* displays saving screen */
	printCmd17,	/* adds evidence to court record with animation and SFX, args: object to add */
	printCmd18,	/* plays new evidence sound with nothing else?, args: ? */
	printCmd19,	/* ? */
	printCmd1A,	/* starts panning the camera from one side of court to the other (always followed by wait 1e?), args: ? */
	printCmd1B,	/* change background image, args: background to display */
	printCmd1C,	/* show textbox(border?), args: 0 show, 1 hide */
	printCmd1D,	/* shifts background?, args: x*256 for direction + pixels/frame */
	printCmd1E,	/* change the character image shown on-screen, args: ?,?,? */
	printCmd1F,	/* instantly hide the character image */
	printCmd20,	/* ? */
	printCmd21,	/* show evidence window with lifebar */
	printCmd22,	/* fades music, args: fadeout/in, frames until 0/full volume */
	printCmd23,	/* ? */
	printCmd24,	/* reset the game to title screen */
	printCmd25,	/* ? */
	printCmd26,	/* shows court-record button, args: 0 show, 1 hide */
	printCmd27,	/* shakes the screen?, args: ? - ? (first seems to be 1e only and second is changing?) */
	printCmd28,	/* display "testimony" animation shown before witness testifies, args: ? */
	printCmd29,	/* returns from a wrong answer to the point in testimony, where objection was called?, args: ? */
	printCmd2A,	/* ?, always followed by endjmp? */
	printCmd2B,	/* ? */
	printCmd2C,	/* jumps to pointer, args: pointer to target */
	printCmd2D,	/* same as p */
	printCmd2E,	/* paragraph, ends textbox, no interaction */
	printCmd2F,	/* display animation (such as "objection!"), args: ? */
	printCmd30,	/* ? */
	printCmd31,	/* makes characer vanish, args: ? */
	printCmd32,	/* ? */
	printCmd33,	/* sets locations available to move to */
	printCmd34,	/* fades to black, args: ? */
	printCmd35,	/* ? */
	printCmd36,	/* ?, unclear description in python */
	printCmd37,	/* ? */
	printCmd38,	/* ? */
	printCmd39,	/* makes blip for characters on map in case 4 appear?, args: blip to show? */
	printCmd3A,	/* ? */
	printCmd3B,	/* Animation related (begin animation?) */
	printCmd3C,	/* Animation related (makes argument blip flash?) */
	printCmd3D,	/* Animation related (stop animation?) */
	printCmd3E,	/* ? */
	printCmd3F,	/* unknown jump, args: ? */
	printCmd40,	/* ? */
	printCmd41,	/* ? */
	printCmd42,	/* toggles play of voice/typewriter sfx, args: 0 play - 1 stop */
	printCmd43,	/* show lifebar (with animation), args: 0 slide out - 1 slide in? */
	printCmd44,	/* play guilty animation, args: ? */
	printCmd45,	/* jump at the end of special messages? */
	printCmd46,	/* change all background tiles, args: tile to change to */
	printCmd47,	/* ? */
	printCmd48,	/* ? */
	printCmd49,	/* return to title, unlock all cases */
	printCmd4A,	/* ?, crash? */
	printCmd4B,	/* ? */
	printCmd4C,	/* ? */
	/* commands following are nullsubs in GS1 gba */
	printCmd4D,	/* ?, not tested? */
	printCmd4E,	/* wait for specified time, no character animation, args: time to wait */
	printCmd4F,	/* ? */
	printCmd50,	/* ? */
	printCmd51,	/* ? */
	printCmd52,	/* ? */
	printCmd53,	/* ? */
	printCmd54,	/* various lifebar related things?, args: ?, ? */
	printCmd55,	/* ?, crash? */
	printCmd56,	/* ? */
	printCmd57,	/* play psychoblock chain and lock appearance animation, args: locks to show */
	printCmd58,	/* ? */
	printCmd59,	/* ? */
	printCmd5A,	/* ? */
	printCmd5B,	/* ? */
	printCmd5C,	/* ?, crash? */
	printCmd5D,	/* toggles text centering, args: 0 normal alignment - 1 centered */
	printCmd5E,	/* ? */
	printCmd5F,	/* ? */
	/* commands introduced after GS1 gba */
	printCmd60,	/* initiates a psychelock item challenge */
	printCmd61,	/* ? */
	printCmd62,	/* ? */
	printCmd63,	/* ?, crash? */
	printCmd64,	/* show special effect?, args: ? */
	printCmd65,	/* ? */
	printCmd66,	/* ? */
	printCmd67,	/* ? */
	printCmd68,	/* ? */
	printCmd69,	/* play fullscreen animation, args: ? */
	printCmd6A,	/* loads new script and jumps to beginning, args: current case? (python is unclear) */
	printCmd6B,	/* Animation related (load animation? used for maps?) */
	printCmd6C,	/* ? */
	printCmd6D,	/* ? */
	printCmd6E,	/* ? */
	printCmd6F,	/* ? */
	printCmd70,	/* ? */
	printCmd71,	/* ? */
	printCmd72,	/* ? */
	printCmd73,	/* ?, crash? */
	printCmd74,	/* ?, crash? */
	printCmd75,	/* ?, crash? */
	printCmd76,	/* ?, crash? */
	printCmd77,	/* ?, crash? */
	printCmd78,	/* unity maps cmd78 to cmd36. possibly something to do with localization? */
	printCmd79,	/* ?, crash? */
	printCmd7A,	/* found in phoenix 1 case 5, jumps if life is zero */
	printCmd7B,	/* ?, crash? */
	printCmd7C,	/* ?, crash? */
	printCmd7D,	/* reset to capcom animation? */
	printCmd7E,	/* ?, crash? */
	printCmd7F,	/* ?, crash? */
	/* all of the following commands were added to support apollo */
	printCmd80,	/* dummy for apollotesting */
	printCmd81,	/* dummy for apollotesting */
	printCmd82,	/* dummy for apollotesting */
	printCmd83,	/* dummy for apollotesting */
	printCmd84,	/* dummy for apollotesting */
	printCmd85,	/* dummy for apollotesting */
	printCmd86,	/* dummy for apollotesting */
	printCmd87,	/* dummy for apollotesting */
	printCmd88,	/* dummy for apollotesting */
	printCmd89,	/* dummy for apollotesting */
	printCmd8A,	/* dummy for apollotesting */
	printCmd8B,	/* dummy for apollotesting */
	printCmd8C,	/* dummy for apollotesting */
	printCmd8D,	/* dummy for apollotesting */
	printCmd8E,	/* dummy for apollotesting */
	printCmd8F,	/* dummy for apollotesting */
};

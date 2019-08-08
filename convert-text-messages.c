#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define sizeofarr(a) (sizeof(a) / sizeof(a[0]))

typedef struct {
	char name[32];
	int args;
}operator;

/* all jump commands have the target encoded as _section numbers_ with 128 added on top */
/* personvanish arg1:
	03 - show
	04 - hide
*/
/* numbers in comments are arguments as stated in MessageSystem.cs from unity */
operator opcodeList[144] = {
	{ "noop", 0 },						/* does nothing */
	{ "b", 0 },						/* linebreak */
	{ "p", 0 },						/* paragraph, ends current textbox, waits for player interaction */
	{ "textcolor", 1 },					/* text color, args: 0 white, 1 red, 2 blue, 3 green */
	{ "pause", 0 },	/* 1 */					/* pause the game, waits for player interaction */
	{ "music", 2 },						/* change the music, args sequence(SDAT index) - fadein time frames (on resume only?) */
	{ "sound", 2 },						/* play a sound effect, args ? - ? */
	{ "fullscreen_text", 0 },				/* switches to fullscreen display, GBA only? */
	{ "finger_choice_2_args_jmp", 2 },			/* select between two choices (either in FS mode with previous opcode or from touchscreen), args: pointers to respective choices in script, followed by endjump? */
	{ "finger_choice_3_args_jmp", 3 },			/* see above with 3 choices */
	{ "rejmp", 1 },						/* pointer to jump to for multiple choice questions failed once */
	{ "speed", 1 },						/* change text speed, args: frames/character */
	{ "wait", 1 },						/* wait for specified time units, args: frames to wait */
	{ "endjmp", 0 },					/* terminates a jump, usually found after all other jumpstatements */
	{ "name", 1 },						/* change the name in the top left of a textbox, apparently, arg needs to be shifted to the right by 8 (actual value in upper 8 bits of 16bit arg) */
	{ "testimony_box", 2 },					/* begins a testimony section, args: ? - ? */
	{ "10", 1  },						/* ? */
	{ "evidence_window_plain", 0 },				/* show evidence window without lifebar (as when pressing R) */
	{ "bgcolor", 3 },					/* flashes the screen? args(?) color, fadein, fadeout in frames? args in python not clear enough :/ */
	{ "showevidence", 1 },					/* displays little evidence box with SFX, args: evidence to show? */
	{ "removeevidence", 0 },				/* removes above box with SFX */
	{ "special_jmp", 0 },					/* used at the end of testimony boxes for saves and resets? */
	{ "savegame", 0 },					/* displays saving screen */
	{ "newevidence", 1 },					/* adds evidence to court record with animation and SFX, args: object to add */
	{ "newevidence_noanim", 1 },				/* plays new evidence sound with nothing else?, args: ? */
	{ "19", 2 },						/* ? */
	{ "swoosh", 4 },					/* starts panning the camera from one side of court to the other (always followed by wait 1e?), args: ? */
	{ "bg", 1 },						/* change background image, args: background to display */
	{ "hidetextbox", 1 },					/* show textbox(border?), args: 0 show, 1 hide */
	{ "shift_background", 1 },				/* shifts background?, args: x*256 for direction + pixels/frame */
	{ "person", 3 },					/* change the character image shown on-screen, args: ?,?,? */
	{ "hideperson", 0 },					/* instantly hide the character image */
	{ "20", 1 },						/* ? */
	{ "evidence_window_lifebar", 0 },			/* show evidence window with lifebar */
	{ "fademusic", 2 },					/* fades music, args: fadeout/in, frames until 0/full volume */
	{ "23", 2 },						/* ? */
	{ "reset", 0 },						/* reset the game to title screen */
	{ "25", 1 },						/* ? */
	{ "hide_court_record_button", 1 },			/* shows court-record button, args: 0 show, 1 hide */
	{ "shake", 2 },						/* shakes the screen?, args: ? - ? (first seems to be 1e only and second is changing?) */
	{ "testemony_animation", 1 },				/* display "testimony" animation shown before witness testifies, args: ? */
	{ "return_to_testimony", 1 },				/* returns from a wrong answer to the point in testimony, where objection was called?, args: ? */
	{ "2A", 3 },						/* ?, always followed by endjmp? */
	{ "2B", 0 },						/* ? */
	{ "jmp", 1 },						/* jumps to pointer, args: pointer to target */
	{ "nextpage_button", 0 },				/* same as p */
	{ "nextpage_nobutton", 0 },				/* paragraph, ends textbox, no interaction */
	{ "animation", 2 },					/* display animation (such as "objection!"), args: ? */
	{ "30", 1 },						/* ? */
	{ "personvanish", 2 },					/* makes characer vanish, args: ? */
	{ "32", 2 },						/* ? */
	{ "33", 2 },	/* 5 */					/* unknown jump, args: ? */
	{ "fadetoblack", 1 },					/* fades to black, args: ? */
	{ "35", 2 },						/* ? */
	{ "36", 0 },	/* 1 */					/* ?, unclear description in python */
	{ "37", 2 },						/* ? */
	{ "38", 1 },						/* ? */
	{ "littlesprite", 1 },					/* makes blip for characters on map in case 4 appear?, args: blip to show? */
	{ "3A", 2 }, /* 3 */					/* ? */
	{ "3B", 2 },						/* Animation related (begin animation?) */
	{ "3C", 1 },						/* Animation related (makes argument blip flash?) */
	{ "3D", 1 },						/* Animation related (stop animation?) */
	{ "3E", 1 },						/* ? */
	{ "3F", 0 },						/* unknown jump, args: ? */
	{ "40", 0 },						/* ? */
	{ "41", 0 },						/* ? */
	{ "togglevoicesfx", 1 },				/* toggles play of voice/typewriter sfx, args: 0 play - 1 stop */
	{ "show_lifebar", 1 },					/* show lifebar (with animation), args: 0 slide out - 1 slide in? */
	{ "guilty", 1 },					/* play guilty animation, args: ? */
	{ "45", 0 },						/* jump at the end of special messages? */
	{ "bgtile", 1 },					/* change all background tiles, args: tile to change to */
	{ "47", 2 },						/* ? */
	{ "48", 2 },						/* ? */
	{ "wingame", 0 },					/* return to title, unlock all cases */
	{ "4A", 0 }, /* 1 */					/* ?, crash? */
	{ "4B", 1 },						/* ? */
	{ "4C", 0 },						/* ? */
	{ "4D", 2 },						/* ?, not tested? */
	{ "wait_noanim", 1 },					/* wait for specified time, no character animation, args: time to wait */
	{ "4F", 7 },						/* ? */
	{ "50", 1 },						/* ? */
	{ "51", 2 },						/* ? */
	{ "52", 1 },						/* ? */
	{ "53", 0 },						/* ? */
	{ "lifebarset", 2 },					/* various lifebar related things?, args: ?, ? */
	{ "55", 2 }, /* 1 */					/* ?, crash? */
	{ "56", 2 },						/* ? */
	{ "psychoblock", 1 },					/* play psychoblock chain and lock appearance animation, args: locks to show */
	{ "58", 0 },						/* ? */
	{ "59", 1 },						/* ? */
	{ "5A", 1 },						/* ? */
	{ "5B", 2 },						/* ? */
	{ "5C", 0 }, /* 3 */					/* ?, crash? */
	{ "toggle_center_text", 1 }, /* 0 */			/* toggles text centering, args: 0 normal alignment - 1 centered */
	{ "5E", 1 }, /* 0 */					/* ? */
	{ "5F", 3 },						/* ? */
	{ "60", 0 }, /* 4 */					/* ?, crash? */
	{ "61", 3 },						/* ? */
	{ "62", 0 },						/* ? */
	{ "63", 0 },						/* ?, crash? */
	{ "64", 1 },						/* show special effect?, args: ? */
	{ "65", 2 },						/* ? */
	{ "66", 2 }, /* 3 */					/* ? */
	{ "67", 0 },						/* ? */
	{ "68", 0 },						/* ? */
	{ "bganim", 2 }, /* 4 */				/* play fullscreen animation, args: ? */
	{ "switchscript", 1 },					/* loads new script and jumps to beginning, args: current case? (python is unclear) */
	{ "6B", 3 },						/* Animation related (load animation? used for maps?) */
	{ "6C", 1 }, /* 0 */					/* ? */
	{ "6D", 1 },						/* ? */
	{ "6E", 1 },						/* ? */
	{ "6F", 1 },						/* ? */
	{ "70", 3 },						/* ? */
	{ "71", 3 },						/* ? */
	{ "72", 0 },						/* ? */
	{ "73", 0 },						/* ?, crash? */
	{ "74", 0 }, /* 2 */					/* ?, crash? */
	{ "75", 4 },						/* ?, crash? */
	{ "76", 0 }, /* 2 */					/* ?, crash? */
	{ "77", 0 }, /* 2 */					/* ?, crash? */
	{ "78", 0 }, /* 1 */					/* ?, crash? */
	{ "79", 0 },						/* ?, crash? */
	{ "7A", 0 }, /* 1 */					/* reset to capcom animation? */
	{ "7B", 0 }, /* 2 */					/* ?, crash? */
	{ "7C", 0 },						/* ?, crash? */
	{ "7D", 0 }, /* 1 */					/* reset to capcom animation? */
	{ "7E", 0 }, /* 1 */					/* ?, crash? */
	{ "7F", 0 }, /* 1 */					/* ?, crash? */
	{ "80", 0 },						/* dummy for apollotesting */
	{ "81", 0 },						/* dummy for apollotesting */
	{ "82", 0 },						/* dummy for apollotesting */
	{ "83", 0 },						/* dummy for apollotesting */
	{ "84", 0 },						/* dummy for apollotesting */
	{ "85", 0 },						/* dummy for apollotesting */
	{ "86", 0 },						/* dummy for apollotesting */
	{ "87", 0 },						/* dummy for apollotesting */
	{ "88", 0 },						/* dummy for apollotesting */
	{ "89", 0 },						/* dummy for apollotesting */
	{ "8A", 0 },						/* dummy for apollotesting */
	{ "8B", 0 },						/* dummy for apollotesting */
	{ "8C", 0 },						/* dummy for apollotesting */
	{ "8D", 0 },						/* dummy for apollotesting */
	{ "8E", 0 },						/* dummy for apollotesting */
	{ "8F", 0 }						/* dummy for apollotesting */
};

// charlist assumes that token is not an opcode and was reduced by 128

char *charset[256] = {
	"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F",
	"G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V",
	"W", "X", "Y", "Z", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l",
	"m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "!", "?",
	"É", "À", "È", "Ù", "Â", "Ê", "Î", "Ô", "Û", "Ë", "Ï", "Ü", "Ç", "Œ", "é", "à",
	"è", "ù", "â", "ê", "î", "ô", "û", "ë", "ï", "ü", "ç", "œ", "°", "€", ";", "á",
	"ä", "å", "æ", "﹚", "ö", "ø", "ß", "ÿ", "Ä", "Å", "Æ", "﹙", "Ö", "Ø", "ì", "ò",
	"Ì", "Ò", "í", "ñ", "ó", "ú", "Á", "Í", "Ñ", "Ó", "Ú", "¿", "¡",  0 ,  0 ,  0 ,
	 0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
	 0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
	 0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
	 0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
	 0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,  0 ,
	"-", "\"", "[", "]", "$", "#", ">", "<", "=", "˽", " ",  0 ,  0 ,  0 ,  0 ,  0 , // space here because the scripts contain enough refs to it, char before space is [box]
	 0 , ".", "☞", "｢", "｣", "(", ")", "『", "』", "‟", "”", "▼", "▲", ":", "`", ",",
	"+", "/", "*", "'", "-", "･", "｡", "%%", "¨", "~", "«", "»", "&", "☆", "♪", " "
};

/*	names for the "person" command
	note that all typos and/or inconsistencies are transcribed as-is from the US releases
	TODO: fix these names, maybe add localization? */

/* zeroes are required because apollo introduces 5 additional slots */
char *speakers[4][60] = {
	{ /* names from phoenix 1 */
	"empty - 00",		"??? - 01",	"Phoenix - 02",	"Police",	"Maya",
	"Mia - 05",		"Alarmclock",	"Mia - 07",	"Judge",	"Edgeworth - 09",
	"Payne",		"Interphone",	"Grossberg",	"Cellular",	"Public",
	"??? - 15",		"Penny",	"Oldbag",	"Manella",	"TV",
	"Gumshoe",		"White",	"April",	"Bellboy",	"Vasquez",
	"Butz - 25",		"Sahwit",	"Will",		"Cody",		"Phoenix - 29",
	"Edgeworth - 30",	"Lotta",	"Yogi",		"Karma",	"Parrot",
	"Missile",		"Uncle",	"Guard - 37",	"Teacher",	"Edgeworth - 39",
	"Butz - 40",		"empty - 41",	"empty - 42",	"Chief",	"Ema",
	"Lana",			"Marshall",	"Meekins",	"Goodman",	"Gant",
	"Angel",		"Guard - 51",	"Officer",	"Patrolman",	"empty - 54",
	0, 0, 0, 0, 0
	},
	{ /* names from phoenix 2 */
	"empty - 00",		"??? - 01",	"??? - 02",		"Phoenix",		"Maya",
	"Mia - 05",		"Mia - 06",	"Judge",		"Edgeworth",		"Payne",
	"Gumshoe",		"Phone",	"Public",		"Bailiff",		"von Karma - 14",
	"von Karma - 15",	"Wellington",	"Byrde - 17",		"Byrde - 18",		"Ini",
	"Pearl",		"Morgan",	"Hotti",		"Grey",			"Lotta",
	"Mia - 25",		"Nurse",	"Mimi",			"Regina",		"Max",
	"Ben",			"Moe",		"Acro",			"Trilo",		"Money",
	"Engarde",		"Andrews",	"de Killer - 37",	"de Killer - 38",	"Oldbag",
	"Powers",		"TV",		"empty - 42",		"empty - 43",		"empty - 44",
	"Celeste",		"Russell",	"Bellboy",		"PA Notice",		"Ray Gun",
	"Chief",		"Detective",	"Jailer",		"Shoe",			"Doe",
	0, 0, 0, 0, 0
	},
	{ /* names from phoenix 3 */
	"Phoenix",	"Mia",		"Grossberg",	"Payne",	"Judge",
	"Dahlia",	"???",		"Swallow",	"Judge",	"Ron",
	"Desirée",	"DeMasque",	"Andrews",	"Godot",	"Atmey",
	"Pearl",	"Announcer",	"Phone",	"Gumshoe",	"Butz",
	"Maya",		"Bailiff",	"Officer",	"Buzzer",	"Armstrong",
	"Basil",	"Viola",	"Tigre",	"Byrde",	"Kudo",
	"Chief",	"Old Man",	"Detective",	"The Tiger",	"Programmer",
	"Valerie",	"Fawles",	"Edgeworth",	"Melissa",	"Armando",
	"Bikini",	"Iris",		"Laurise",	"Elise",	"von Karma",
	"Morgan",	"Misty",	"Oldbag",	"Ray Gun",	"blank - 49",
	"Violetta",	"blank - 51",	"blank - 52",	"blank - 53",	"blank - 54",
	0, 0, 0, 0, 0
	},
	{ /* names from apollo */
	"empty - 00",		"empty - 01",		"??? - 02",		"??? - 03",		"Apollo",
	"Trucy - 05",		"Kristoph - 06",	"Phoenix",		"Kristoph - 08",	"Ema",
	"Judge",		"Payne",		"Olga",			"Alita",		"Wocky",
	"Guy",			"Plum",			"Big Wins",		"Hickfield",		"Vera - 19",
	"Brushel",		"Phoenix",		"Gumshoe",		"Kristoph - 23",	"Kristoph - 24",
	"Valant",		"Drew",			"Trucy - 27",		"Vera - 28",		"Meekins",
	"Stickler",		"Zak",			"Smith",		"Valant",		"Lamiroir",
	"Machi",		"Le Touse",		"Daryan",		"PLACEHOLDER - 38",	"PLACEHOLDER - 39",
	"PLACEHOLDER - 40",	"PLACEHOLDER - 41",	"PLACEHOLDER - 42",	"Bailiff",		"Officer",
	"Announcer",		"Mr. Hat",		"Meraktis",		"Enigmar",		"Public",
	"Magnifi",		"No. 6",		"Guard",		"Klavier",		"Thalassa",
	"Staff",		"Old Lady",		"Kristoph - 57",	"TV",			"blank - 59"
	}
	
};

char *colors[] = {
	"white", "red", "blue", "green"
};

char *shiftdirection[] = {
	"left", "right", "up", "down"
};

/* multilang backgrounds share their number! */
/* TODO: add support for multiple games */
/* 4096 is the size per game due to BGSPC, this may change due to various weird leftover commands though */
char *backgrounds[4][4096] = {
	{
		/* phoenix 1 */
		"BG000 - Mias Office Night",
		"BG001 - Mias Office Day",
		"BG002 - Defendant Lobby",
		"BG003 - Court Defense",
		"BG004 - Court Prosecution",
		"BG005 - Court Stand",
		"BG006 - Court Full",
		"BG007 - Court Defense Partner",
		"BG008 - Court Judge",
		"BG009 - Mias Office Entrance",
		"BG010 - Dead Mia with Maya",
		"BG011 - Global Studios Path", 
		"BG012 - Staff Area",
		"BG013 - Mia dodging right",
		"BG014 - Mia dodging left",
		"BG015 - Map Of Mias Office",
		"BG016 - April and Bellboy",
		"BG017 - Mias Office Body",
		"BG018 - Phoenix with Maya and Mia",
		"BG019 - Studio 2 Inside",
		"BG020 - April on phone",
		"BG021 - Redd attacking", 
		"BG022 - Mia being attacked",
		"BG023 - Thinker as weapon",
		"BG024 - Dressing Room", 
		"BG025 - Global Studios Main Gate", 
		"BG026 - Studio 1",
		"BG027 - Gavel Slam",
		0,
		"BG029 - Map Of Mias Office With FOV",
		"BG030 - Detention Center",
		"BG031 - Bluecorp CEO Office",
		"BG032 - Gatewater Aprils Room",
		"BG033 - Thinker with blood",
		"BG034 - Cindy dead on floor",
		"BG035 - Sahwit after murder",
		"BG036 - Sahwit clueless",
		"BG037 - Sahwit thinking",
		"BG038 - Sahwit having idea",
		"BG039 - Grossbergs Office",
		"BG040 - Grossbergs Office no Painting",
		"BG041 - Sahwit caught",
		"BG042 - Sahwit seeing Larry",
		"BG043 - Sahwit finding Cindy",
		"BG044 - Studio 2 Entrance", 
		0, 0, 0, 0, 
		"BG049 - Steel Samurai Broken Leg",
		"BG050 - Finding Dead Hammer",
		"BG051 - Hammer Stealing Costume",
		"BG052 - Vasquez After Murder", 
		"BG053 - Staff At Lunch", 
		"BG054 - Hammer And Vasquez At Studio 2", 
		"BG055 - Cody Sees Samurai",
		"BG056 - Photo Of Samurai At Studio 2", 
		"BG057 - Incident At Studio 2 5 Years ago", 
		"BG058 - Studio 2 Inside With MIB", 
		"BG059 - Pink Princess Poster", 
		"BG060 - Monkey Head On Studio Path",
		"BG061 - Map Of Global Studios", 
		"BG062 - Actors Training", 
		"BG063 - Steel Samurai Poster", 
		0, 0,
		"BG066 - Closeup Speedlines",
		0, 0,
		"BG069 - Steel Samurai Moon",
		"BG070 - Gourd Lake Entrance",
		"BG071 - Gourd Lake Park Decorated",
		"BG072 - Gourd Lake Park",
		"BG073 - Gourd Lake Forest",
		"BG074 - Steel Samurai Moon Grass",
		0, 
		"BG076 - Steel Samurai In Action",
		"BG077 - Gourd Lake Boat Rental",
		"BG078 - Gourd Lake Boat Rental Inside",
		"BG079 - Police Station Inside", 
		"BG080 - Police Station Evidence Room",
		"BG081 - Steel Samurai Crossing Swords",
		0, 0, 0,
		"BG085 - Train Station",
		"BG086 - Map Of Gourd Lake",
		"BG087 - Lottas Photo Enlarged",
		"BG088 - Incident From DL-6",
		"BG089 - Newspaper About Gourdy",
		"BG090 - Larry With Balloon",
		"BG091 - Balloon Hitting Lake",
		"BG092 - Larry Searching For Balloon",
		"BG093 - School Court",
		"BG094 - Lottas Photo",
		"BG095 - School Phoenix",
		"BG096 - School Edgeworth",
		"BG097 - School Larry",
		"BG098 - Yanni Attacking Victim",
		"BG099 - Yanni And Edgeworth On Boat",
		"BG100 - Edgeworth Inside Elevator",
		"BG101 - Edgeworth Throwing Gun",
		"BG102 - Von Karma Being Shot",
		"BG103 - Von Karma Entering Elevator",
		"BG104 - Maya Leaving At Station",
		"BG105 - Winning Party After Credits",
		"BG106 - Boat With Single Passenger",
		"BG107 - Man With Gun",
		"BG108 - Edgeworth With Gun",
		"BG109 - Capcom Logo Black And White",
		0, 0, 0,
		"BG113 - Police Station Entrance",
		"BG114 - Edgeworths Office",
		"BG115 - Parking Garage",
		"BG116 - Gants Office",
		"BG117 - Evidence Locker Room",
		"BG118 - Police Station Security Room",
		"BG119 - Map Of Parking Garage",
		"BG120 - Lana Attacking Goodman",
		"BG121 - Lana In Garage",
		"BG122 - Angel With Lana",
		"BG123 - Edgeworths Car Trunk",
		"BG124 - Goodmans Note",
		"BG125 - Key Card Log Blank",
		"BG126 - Meekins With Goodman At Lockers",
		0,
		"BG128 - Bloody Print On Locker",
		"BG129 - Luminol Print On Locker",
		"BG130 - Hand Print On Cloth",
		0, 0, 0, 0, 0, 0, 0, 0, 0,
		"BG140 - Contradiction Of The Law",
		0,
		"BG142 - Vase Closeup Of Writing",
		0,
		"BG144 - Evidence Lockers Closeup",
		"BG145 - Evidence Lockers Closeup Of Print",
		"BG146 - Evidence Lockers Left Luminol 0",
		"BG147 - Evidence Lockers Left Luminol 1",
		0, 0,
		0, 0, 0, 
		"BG153 - Vase Break 1",
		"BG154 - Vase Break 2",
		"BG155 - Vase Break 3",
		"BG156 - Backlit Badger",
		"BG157 - Parking Garage Background",
		0,
		"BG159 - Map Of Evidence Locker Room",
		"BG160 - Vase Closeup With Bloody Writing Complete",
		"BG161 - Key Card Log 2",
		"BG162 - Key Card Log 3",
		0, 0, 0, 0,
		"BG167 - Kurain Village",
		0, 0,
		//170
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		//180
		0,
		"BG181 - Statue In Gants Office",
		"BG182 - Lana And Gant",
		"BG183 - SL-9 Murderer Darke",
		"BG184 - SL-9 Investigation Group",
		"BG185 - SL-9 Gant Lana Neil With Trophy",
		"BG186 - SL-9 Murder Scene",
		"BG187 - SL-9 Lana With Unconcious Ema",
		"BG188 - SL-9 Dead Neil With Murderer",
		"BG189 - SL-9 Emas Drawing",
		"BG190 - SL-9 Emas Drawing People",
		"BG191 - SL-9 Emas Drawing Vase",
		0, 0, 0, 0, 0,
		"BG197 - Vase With Bloody Writing",
		0,
		"BG199 - Neil Impaled On Statue Photo",
		"BG200 - Japanese Keypad", // matching language is just patched on?
		"BG201 - Evidence Lockers Untouched",
		"BG202 - Red Badger Head",
		"BG203 - Lana Hugging Ema",
		0, 0, 0, 0, 0, 0,
		//210
		0,
		0, //211 this is gants office again?
		"BG212 - Vase Closeup With Missing Piece",
		0, 0, 0, 0, 0, 0, 0,
		//220
		0,
		"BG221 - Vase Angled As Badger",
		"BG222 - Keypad Display",
		"BG223 - Safe At Gants Office 0",
		"BG224 - Safe At Gants Office 1",
		"BG225 - Safe At Gants Office 2",
		"BG226 - Vase Missing Piece Complete Writing",
		0,
		"BG228 - City In Intro",
		0,
		"BG230 - Background For Dancing Badger",
		0,
		"BG232 - King Of Prosecutors Trophy",
		0,
		"BG234 - Emas Drawing Vertical",
		"BG235 - Police Station Entrance No Badger",
		"BG236 - Neil Flying Towards Statue 1",
		"BG237 - Neil Flying Towards Statue 2",
		"BG238 - Neil Flying Towards Statue 3",
		0,
		"BG240 - Goodman In Car Trunk",
		0, 0, 0, 0, 0, 0, 0, 0, 0,
		//250
		0, 0, 0, 0, 0,
		0, //255
		0, 0, 0, 0,
		
		[4095] = "BGSPC - Black Screen"
		// 32884 seems to be gants office yet again?
	},
	{
		/* phoenix 2 to be done */
		0,
		0,
		"BG002 - Capcom Logo grayscale",
		"BG003 - Defense Lobby",
		"BG004 - Court Defense",
		"BG005 - Court Prosecution",
		"BG006 - Court Stand",
		"BG007 - Court Judge",
		"BG008 - Court Defense Partner",
		"BG009 - Courtroom Full View",
		0,
		"BG011 - Gavel Slam",
		0,
		0,
		0,
		0,
		"BG016 - Speedlines",
		"BG017 - Detention Center",
		"BG018 - Phoenix Office",
		"BG019 - Police HQ Inside",
		0,
		"BG021 - Photo of dead Dustin",
		"BG022 - Photo of Message in Sand",
		"BG023 - Maggey with Victim",
		"BG024 - Maggey sees Victim and Culprit",
		0,
		"BG026 - Phoenix Nightmare",
		"BG027 - Culprit after hitting Phoenix",
		"BG028 - Kurain",
		"BG029 - Fey Manor Main Hall",
		"BG030 - Fey Manor Summoning Room",
		"BG031 - Fey Manor Winding Way",
		"BG032 - Fey Manor Spare Room",
		0,
		"BG034 - Hotti Clinic",
		"BG035 - Car driving",
		"BG036 - Car accident",
		"BG037 - Phoenix with Maya in detention",
		0,
		0,
		0,
		0,
		"BG042 - Map of Summoning Room",
		"BG043 - Map of Fey Manor",
		"BG044 - Car lights right",
		"BG045 - Dead Mia from Phoenix 1",
		"BG046 - Maya with Victim",
		"BG047 - Nurse at Hospital",
		"BG048 - Mayas Costume",
		"BG049 - Photo of Culprit back",
		"BG050 - Photo of Culprit front",
		"BG051 - Photo of Mia summoned by Maya",
		"BG052 - Pearl fixing Urn",
		"BG053 - Morgan with Photo",
		"BG054 - Culprit and Ini",
		"BG055 - Ini with knocked out Maya",
		"BG056 - Box behind Screen",
		"BG057 - Victim with Gun",
		"BG058 - Maya and Mia summoned by Pearl",
		"BG059 - Circus Tent",
		"BG060 - Circus Cafeteria",
		"BG061 - Mr. Berries Office",
		0,
		"BG063 - Circus Entrance Night",
		"BG064 - Circus Courtyard",
		"BG065 - Moes Room",
		"BG066 - Acros Room",
		0,
		0,
		"BG069 - Circus Full View",
		"BG070 - Max Galactica Intro",
		0,
		"BG072 - Max Galactica Poster",
		"BG073 - Photo of Max with Award",
		"BG074 - Max negotiating with Victim",
		"BG075 - Map of Circus",
		"BG076 - Photo of Victim over Box",
		"BG077 - Victim slumped over Box",
		"BG078 - Victim lifting Box",
		"BG079 - Victim hit with Statue",
		"BG080 - Lion with Acros brother",
		"BG081 - Victim seen by Trilo",
		"BG082 - Acro seeing Max",
		"BG083 - Max Statue",
		"BG084 - Acro dropping Statue",
		"BG085 - Max grabbing Bottle",
		"BG086 - Acro lower half at Court",
		"BG087 - Edgeworth at Airport",
		"BG088 - Gatewater Hotel Ballroom",
		"BG089 - Gatewater Hotel Hallway",
		"BG090 - Gatewater Hotel Engardes Room",
		"BG091 - Gatewater Hotel Corridas Room",
		0,
		"BG093 - Gatewater Hotel Lobby",
		"BG094 - Engardes Living Room",
		"BG095 - Secret Room",
		"BG096 - Wine Cellar",
		"BG097 - Clouds",
		"BG098 - Jammin Ninja Poster",
		"BG099 - Airport",
		"BG100 - Photo of Victim",
		0,
		"BG102 - Photo of Andrews in Costume",
		"BG103 - Nickel Samurai Poster",
		"BG104 - Juan at Suicide",
		"BG105 - Andrews dressing up in Costume",
		"BG106 - Andrews waking up Matt",
		"BG107 - Engarde with Celeste base",
		"BG108 - Edgeworth with Gun from Phoenix 1",
		"BG109 - Engarde with Butler",
		"BG110 - Franziska entering Court",
		"BG111 - Butler recieving Bear",
		"BG112 - Phoenix with Maya and Pearl",
		"BG113 - Calling Card with Drawing",
		"BG114 - Phoenix leaving Bad End",
		"BG115 - Andrews finding Juan",
		"BG116 - De Killer Calling Card",
		"BG117 - Steel Samurai Poster",
		0,
		"BG119 - Young Edgeworth from Phoenix 1",
		"BG120 - Heroes lineup",
		"BG121 - Nickel Samurai Spotlight",
		"BG122 - Nickel Samurai with Fan",
		
		
		[4095] = "BGSPC - Black Screen",
	},
	{
		/* phoenix 3 to be done */
		0
	},
	{
		/* apollo to be done */
		0
	}
};

char *musicfading[] = {
	"fadeout", "fadein"
};

char *animationstate[] = {
	"stop", "start"
};

/* TODO: add support for multiple games */
/* 1024 is a placeholder, change when all things are known */
char *sound_data[4][1024] = {
	{
		/* phoenix 1 */
		/* BEGIN BGM */
		"BGM000 - Detention Center",
		"BGM001 - Gumshoes Theme",
		"BGM002 - Cornered",
		"BGM003 - Cornered Variation",
		"BGM004 - Objection",
		"BGM005 - Logic and Trick",
		"BGM006 - Mayas Theme",
		"BGM007 - Ending",
		"BGM008 - Courtroom Lobby",
		"BGM009 - Unused",
		"BGM010 - Questioning Moderato",
		"BGM011 - Questioning Allegro",
		"BGM012 - Investigation Core",
		"BGM013 - Courtroom Opening",
		"BGM014 - Jingle Surprise",
		"BGM015 - Jingle Evidence",
		"BGM016 - Jingle Save",
		"BGM017 - Jingle Idea",
		"BGM018 - Suspense",
		"BGM019 - Turnabout Sisters Ballad", 
		"BGM020 - Victory",
		"BGM021 - Steel Samurai Theme",
		"BGM022 - Happy People",
		"BGM023 - Intro Case 1",
		"BGM024 - Telling the Truth",
		"BGM025 - Investigation Opening",
		"BGM026 - Grossbergs Theme",
		"BGM027 - Recollection DL-6",
		"BGM028 - Recollection Maya",
		"BGM029 - Recollection Class Trial",
		"BGM030 - Recollection Studio",
		"BGM031 - Ambient Bird Chirps",
		"BGM032 - Jingle Wrong Choice",
		"BGM033 - Unknown",
		"BGM034 - Unknown",
		"BGM035 - Unknown",
		"BGM036 - Unknown",
		"BGM037 - Unknown",
		"BGM038 - Unknown",
		"BGM039 - Unknown",
		"BGM040 - Unknown",
		"BGM041 - Unknown",
		/* END BGM */
		/* BEGIN SE */
		"SE000 - Menu Change",
		"SE001 - Menu Confirm",
		"SE002 - Menu Cancel",
		"SE003 - Text Blip Male",
		"SE004 - Text Blip Female",
		"SE005 - Text Advance",
		"SE006 - Text Blip Typewriter High",
		"SE007 - Menu Open Submenu",
		0,
		"SE009 - Display Evidence",
		"SE00A - Unknown",
		"SE00B - Unknown",
		"SE00C - Menu Change Page",
		"SE00D - Voice Phoenix Take That JP",
		"SE00E - Voice Edgeworth Objection JP",
		"SE00F - Voice Payne Objection JP",
		"SE010 - Gavel Slam",
		"SE011 - Gavel Slam Triple",
		"SE012 - Gavel Slam Hexa",
		"SE013 - Courtroom Audience 1",
		"SE014 - Courtroom Audience 2",
		"SE015 - Courtroom Audience Uproar",
		"SE016 - Confirm Save",
		"SE017 - Voice Von Karma Objection JP",
		"SE018 - Photo Snapshot",
		"SE019 - Impact",
		"SE01A - Text Typewriter Low",
		"SE01B - Crash",
		"SE01C - Desk Hit",
		"SE01D - Voice Phoenix Hold It JP",
		"SE01E - Exclamation",
		"SE01F - Police Siren",
		"SE020 - Door Open Impact",
		"SE021 - Collapse Impact",
		"SE022 - Lost a Try",
		"SE023 - Typewriter Low",
		"SE024 - Critical Hit",
		"SE025 - Gunshot 1",
		"SE026 - Verbal Hit",
		"SE027 - Voice Phoenix Objection JP",
		"SE028 - Crash into Water",
		"SE029 - Begin Questioning",
		"SE02A - Text Typewriter Low",
		"SE02B - Text Typewriter High",
		"SE02C - Game Over",
		"SE02D - Unexpected Turn of Events",
		"SE02E - Unknown",
		"SE02F - Unknown",
		"SE030 - Phone Ring",
		"SE031 - Phone Pickup",
		"SE032 - Phone End of Recording",
		"SE033 - Camera Sound Zoom In",
		"SE034 - Dog Bark",
		"SE035 - Von Karma Snap",
		"SE036 - Japanese Wood Sound",
		"SE037 - Unknown (Earthquake?)",
		"SE038 - Japanese Hit",
		"SE039 - Stick Snap",
		"SE03A - Electro Shock",
		"SE03B - Fire Alarm",
		"SE03C - Courtroom Audience Victory",
		"SE03D - Unknown",
		"SE03E - Redd Bling Sparkles",
		"SE03F - Crash Through Door",
		0,
		"SE041 - Phone Ringtone Steel Samurai",
		0,
		"SE043 - Unknown",
		
		[121] = "SE04F - Swoosh",
		"SE050 - Shocking Turn of Events",
		
		[157] = "SE073 - Stomach Grumble",
		
		/* this seems to be a thing */
		[256] = "BGM-SP - ResumePlaying",
		
		[380] = "BGM150 - Emas Theme",
		"BGM151 - Marshalls Theme",
		"BGM152 - Recollection SL-9",
		"BGM153 - Gants Theme",
		"BGM154 - Intro Case 5",
		"BGM155 - Ending Case 5",
		"BGM156 - Blue Badgers Theme",
		
		[400] = "SE0B0 - Voice Phoenix Objection EN",
		"SE0B1 - Voice Phoenix Hold It EN",
		"SE0B2 - Voice Payne Objection EN",
		"SE0B3 - Voice Edgeworth Objection EN",
		"SE0B4 - Voice Von Karma Objection EN",
		"SE0B5 - Voice Phoenix Take That EN",
		"SE0B6 - Courtroom Audience 3",
		"SE0B7 - Slap",
		"SE0B8 - Gunshot 2",
		"SE0B9 - Thunder",
		"SE0BA - Metal Clang",
		"SE0BB - Organ",
		"SE0BC - Lock Open - Lightswitch",
		"SE0BD - Electronic Interaction",
		"SE0BE - Electronic Confirmation",
		"SE0BF - Electronic Entry",
		"SE0C0 - Fingerprint Match",
		"SE0C1 - Electromechanic Entry",
		"SE0C2 - Meekins Loudspeaker",
		"SE0C3 - Unknown",
		"SE0C4 - Fingerprint Dusting",
		"SE0C5 - Open",
		"SE0C6 - Fingerprint Checking",
		"SE0C7 - Fingerprint Point",
		"SE0C8 - Videoplayer PlayPause",
		"SE0C9 - Videoplayer Seek",
		"SE0CA - Videoplayer Step 1",
		"SE0CB - Videoplayer Step 2",
		"SE0CC - Vase Puzzle Match 1",
		"SE0CD - Vase Puzzle Match 2",
		"SE0CE - Safe Panel Sound 1",
		"SE0CF - Safe Panel Sound 2",
		"SE0D0 - Safe Panel Sound 3",
		"SE0D1 - Safe Panel Sound 4",
		"SE0D2 - Safe Panel Sound 5",
		"SE0D3 - Safe Panel Sound 6",
		"SE0D4 - Case 5 Intro Window Swoosh",
		"SE0D5 - Case 5 Intro Rain 1",
		"SE0D6 - Case 5 Intro Thunder 1",
		0,
		"SE0D8 - Case 5 Intro Lightning 1",
		"SE0D9 - Case 5 Intro Lightning 2",
		"SE0DA - Case 5 Intro Lightning 3",
		"SE0DB - Case 5 Intro Thunder 2",
		"SE0DC - Case 5 Intro Rain 2",
		"SE0DD - Case 5 Intro Thunder 3",
		"SE0DE - Case 5 Intro Swing",
		"SE0DF - Case 5 Intro Flying Object",
		"SE0E0 - Case 5 Intro Shatter",
		"SE0E1 - Case 5 Intro Shock",
		"SE0E2 - Case 5 Intro Thunder 4",
		"SE0E3 - Unknown",
		"SE0E5 - Unknown",
		"SE0E4 - Gunshot 3",
		"SE0E6 - Bottle Open",
		"SE0E7 - Gavel Slam 2",
		"SE0E8 - Meekins Loudspeaker Squeak",
		0,
		"SE0EA - Voice Phoenix Objection FR",
		"SE0EB - Voice Phoenix Hold It FR",
		"SE0EC - Voice Payne Objection FR",
		"SE0ED - Voice Edgeworth Objection FR",
		"SE0EE - Voice Von Karma Objection FR",
		"SE0EF - Voice Phoenix Take That FR",
		"SE0F0 - Voice Phoenix Objection IT",
		"SE0F1 - Voice Phoenix Hold It IT",
		"SE0F2 - Voice Phoenix Take That IT",
		"SE0F3 - Voice Payne Objection IT",
		"SE0F4 - Voice Edgeworth Objection IT",
		"SE0F5 - Voice Von Karma Objection IT",
		"SE0F6 - Voice Phoenix Objection DE",
		"SE0F7 - Voice Phoenix Hold It DE",
		"SE0F8 - Voice Phoenix Take That DE",
		"SE0F9 - Voice Payne Objection DE",
		"SE0FA - Voice Edgeworth Objection DE",
		"SE0FB - Voice Von Karma Objection DE",
		"SE0FC - Voice Phoenix Objection ES",
		"SE0FD - Voice Phoenix Hold It ES",
		"SE0FE - Voice Phoenix Take That ES",
		"SE0FF - Voice Payne Objection ES",
		"SE100 - Voice Edgeworth Objection ES",
		"SE101 - Voice Von Karma Objection ES",
		/* this table lacks various clips from the euro version
		such as IT, ES and DE voiceclips */
	},
	{
		/* phoenix 2 */
		/* everything up to BGM041 is from phoenix 1 */
		
		[1] = "BGM001 - Gumshoes Theme",
		"BGM002 - Cornered 1",
		
		[6] = "BGM006 - Mayas Theme",
		"BGM007 - Ending",
		0,
		"BGM009 - Unused",
		0, 0,
		"BGM012 - Investigation Core 1",
		0,
		"BGM014 - Jingle Surprise",
		"BGM015 - Jingle Evidence",
		"BGM016 - Jingle Save",
		"BGM017 - Jingle Idea",
		"BGM018 - Suspense 1",
		"BGM019 - Turnabout Sisters Ballad 1",
		0,
		"BGM021 - Steel Samurai Theme",
		"BGM022 - Happy People",
		"BGM023 - Intro Case 1",
		0,
		"BGM025 - Investigation Opening 1",
		"BGM026 - Grossbergs Theme",
		"BGM027 - Recollection DL-6",
		"BGM028 - Recollection Maya",
		"BGM029 - Recollection Class Trial",
		"BGM030 - Recollection Studio",
		"BGM031 - Ambient Bird Chirps",
		"BGM032 - Jingle Wrong Choice",
		"BGM033 - Unknown",
		"BGM034 - Unknown",
		"BGM035 - Unknown",
		"BGM036 - Unknown",
		"BGM037 - Unknown",
		"BGM038 - Unknown",
		"BGM039 - Unknown",
		"BGM040 - Unknown",
		"BGM041 - Unknown",
		
		
		
		"SE000 - Menu Change",
		"SE001 - Menu Confirm",
		"SE002 - Menu Cancel",
		"SE003 - Text Blip Male",
		"SE004 - Text Blip Female",
		"SE005 - Text Advance",
		0,
		"SE007 - Menu Open Submenu",
		0,
		"SE009 - Display Evidence",
		"SE00A - Unknown",
		0,
		"SE00C - Menu Change Page",
		"SE00D - Voice Phoenix Take That JP",
		"SE00E - Voice Edgeworth Objection JP",
		"SE00F - Voice Payne Objection JP",
		"SE010 - Gavel Slam",
		"SE011 - Gavel Slam Triple",
		0,
		"SE013 - Courtroom Audience 1",
		0,
		0,
		"SE016 - Confirm Save",
		0,
		0,
		"SE019 - Impact",
		"SE01A - Text Typewriter Low",
		0,
		"SE01C - Desk Hit",
		"SE01D - Voice Phoenix Hold It JP",
		"SE01E - Exclamation",
		0,
		"SE020 - Door Open Impact",
		"SE021 - Collapse Impact",
		"SE022 - Lost a Try",
		0,
		"SE024 - Critical Hit",
		"SE025 - Gunshot 1",
		"SE026 - Verbal Hit",
		"SE027 - Voice Phoenix Objection JP",
		0,
		"SE029 - Begin Questioning",
		0,
		0,
		"SE02C - Game Over",
		"SE02D - Unexpected Turn of Events",
		0,
		0,
		0,
		0,
		0,
		"SE033 - Camera Sound Zoom In",
		"SE034 - Dog Bark",
		"SE035 - Von Karma Snap",
		"SE036 - Japanese Wood Sound",
		"SE037 - Unknown (Earthquake?)",
		"SE038 - Japanese Hit",
		"SE039 - Stick Snap",
		"SE03A - Electro Shock",
		"SE03B - Fire Alarm",
		"SE03C - Courtroom Audience Victory",
		"SE03D - Unknown",
		"SE03E - Redd Bling Sparkles",
		"SE03F - Crash Through Door",
		"SE040 - Phone Ringtone Wellington",
		"SE041 - Phone Ringtone Steel Samurai",
		"SE042 - Short Beep",
		"SE043 - Unknown",
		"SE044 - Unknown",
		"SE045 - Gavel Slam Reverb",
		"SE046 - Whip",
		0,
		0,
		"SE049 - Psyche Lock Slam",
		"SE04A - Psyche Lock Break",
		"SE04B - Car Crash",
		"SE04C - Unknown Door Open",
		"SE04D - Wood Door Impact",
		0,
		"SE04F - Swoosh",
		"SE050 - Shocking Turn of Events",
		"SE051 - Lion Roar",
		"SE052 - Monkey Screech",
		"SE053 - Spot Light On",
		"SE054 - Unknown Aha",
		"SE055 - Wood Door Breakdown",
		"SE056 - Phyche Lock Chains 1",
		"SE057 - Phyche Lock Chains 2",
		"SE058 - Phyche Lock Chains 3",
		"SE059 - Phyche Lock Chains 4",
		"SE05A - Phyche Lock Chains 5",
		"SE05B - Car Engine",
		"SE05C - Car Swoop LeftRight",
		"SE05D - Car Swoop RightLeft",
		"SE05E - Car Engine Crescendo",
		"SE05F - Circus Drumroll",
		"SE060 - Circus Drum",
		"SE061 - Unknown Circus SE",
		"SE062 - Low Audience Cheers",
		"SE063 - High Audience Cheers Long",
		"SE064 - Circus Whip",
		"SE065 - Long Lion Roar",
		"SE066 - High Audience Cheers Short",
		"SE067 - Oldbag Lazerblaster",
		"SE068 - Radio Transmitter Ringtone",
		"SE069 - Radio Transmitter Pickup",
		"SE06A - Radio Transmitter End",
		"SE06B - Radio Transmitter Interference",
		"SE06C - Voice Franziska Objection JP",
		"SE06D - Phone Caller Quit",
		"SE06E - Franziskas Tracker?",
		"SE06F - Unknown",
		"SE070 - Door Unlock",
		"SE071 - Cat Meow",
		"SE072 - Life Bar Restore",
		"SE073 - Stomach Growl",
		"SE074 - Phone Dial 1",
		"SE075 - Phone Dial 2",
		"SE076 - Phone Dial 3",
		"SE077 - Phone Dial 4",
		"SE078 - Phone Dial 5",
		"SE079 - Courtroom Audience 2",
		"SE07A - Courtroom Audience 3",
		"SE07B - Courtroom Audience Fade",
		"SE07C - Courtroom Headline In",
		"SE07D - Courtroom Headline Out",
		0,
		"SE07F - Unknown Aha?",
		"SE080 - Unknown Growl",
		"SE081 - High Audience Cheer Loop",
		"SE082 - Radio Detector Low",
		"SE083 - Phone Dialling",
		"SE084 - Radio Detector Medium",
		"SE085 - Unknown Slam",
		"SE086 - Psyche Lock Break 2?",
		
		[201] = "BGM051 - Intro Case 1 Part 1",
		"BGM052 - Intro Case 3 And 4",
		"BGM053 - Steel Samurais Ballad",
		"BGM054 - Turnabout Sisters Theme 2",
		"BGM055	- Berry Big Circus",
		0,
		"BGM057 - Kurain Village",
		"BGM058 - Detention Center",
		"BGM059 - Eccentric",
		0,
		"BGM061 - Shelly De Killer Theme",
		"BGM062 - Recollection Feys",
		"BGM063 - Recollection Andrews",
		"BGM064 - Investigation Core 2",
		"BGM065 - Investigation Opening 2",
		0,
		"BGM067 - Edgeworths Theme",
		"BGM068 - Annoying People (Moe, Oldbag)",
		"BGM069 - Courtroom Opening 2",
		"BGM070 - Objection 2",
		"BGM071 - Trick And Magic",
		"BGM072 - Questioning Moderato 2",
		"BGM073 - Questioning Allegro 2",
		"BGM074 - Cornered 2",
		"BGM075 - Cornered 2 Variation",
		"BGM076 - Victory 2",
		"BGM077 - Courtroom Lobby 2",
		"BGM078 - Telling the Truth 2",
		0,
		"BGM080 - Jingle Save 2",
		0,
		0,
		"BGM083 - Ending 2 Part 1",
		"BGM084 - Intro Case 1 Part 2",
		"BGM085 - Psyche Lock 2",
		"BGM086 - Hotline of Fate",
		"BGM087 - Franziskas Theme",
		"BGM088 - Investigation Middle 2",
		"BGM089 - Edgeworth Outro",
		"BGM090 - Audience BGM?",
		"BGM091 - Franziskas Theme Variation",
		"BGM092 - Pearls Theme",
		"BGM093 - Ending 2 Part 1 Variation",
		"BGM094 - Ending 2 Part 2",
		
		/* this seems to be a thing */
		[256] = "BGM-SP - ResumePlaying",
		
		[400] = "SE0B0 - Voice Phoenix Objection EN",
		"SE0B1 - Voice Phoenix Hold It EN",
		"SE0B2 - Voice Payne Objection EN",
		"SE0B3 - Voice Edgeworth Objection EN",
		0,
		"SE0B5 - Voice Phoenix Take That EN",
		"SE0B6 - Voice Franziska Objection EN",
		0,
		"SE0B8 - Voice Franziska Objection FR",
		0,
		0,
		0,
		"SE0BC - Voice Franziska Objection DE",
		0,
		0,
		"SE0BF - Voice Franziska Objection IT",
		0,
		0,
		0,
		"SE0C3 - Voice Franziska Objection ES",
		"SE0EA - Voice Phoenix Objection FR",
		"SE0EB - Voice Phoenix Hold It FR",
		"SE0EF - Voice Phoenix Take That FR",
		"SE0EC - Voice Payne Objection FR",
		"SE0ED - Voice Edgeworth Objection FR",
		"SE0F0 - Voice Phoenix Objection IT",
		"SE0F1 - Voice Phoenix Hold It IT",
		"SE0F2 - Voice Phoenix Take That IT",
		"SE0F3 - Voice Payne Objection IT",
		"SE0F4 - Voice Edgeworth Objection IT",
		"SE0F6 - Voice Phoenix Objection DE",
		"SE0F7 - Voice Phoenix Hold It DE",
		"SE0F8 - Voice Phoenix Take That DE",
		"SE0F9 - Voice Payne Objection DE",
		"SE0FA - Voice Edgeworth Objection DE",
		"SE0FC - Voice Phoenix Objection ES",
		"SE0FD - Voice Phoenix Hold It ES",
		"SE0FE - Voice Phoenix Take That ES",
		"SE0FF - Voice Payne Objection ES",
		"SE100 - Voice Edgeworth Objection ES",
	},
	{
		/* phoenix 3 to be done */
		/* everything up to BGM041 is from phoenix 1 */
		
		[1] = "BGM001 - Gumshoes Theme",
		"BGM002 - Cornered 1",
		"BGM003 - Cornered 1 Variation",
		
		[12] = "BGM012 - Investigation Core 1",
		0,
		"BGM014 - Jingle Surprise",
		"BGM015 - Jingle Evidence",
		0,
		0,
		"BGM018 - Suspense 1",
		"BGM019 - Turnabout Sisters Ballad 1",
		0,
		0,
		"BGM022 - Happy People",
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		"BGM031 - Ambient Bird Chirps 1",
		
		[42] = "SE000 - Menu Change",
		"SE001 - Menu Confirm",
		"SE002 - Menu Cancel",
		"SE003 - Text Blip Male",
		"SE004 - Text Blip Female",
		"SE005 - Text Advance",
		0,
		"SE007 - Menu Open Submenu",
		0,
		"SE009 - Display Evidence",
		"SE00A - Unknown",
		0,
		"SE00C - Menu Change Page",
		"SE00D - Voice Phoenix Take That FR",
		"SE00E - Voice Edgeworth Objection FR",
		"SE00F - Voice Payne Objection FR",
		"SE010 - Gavel Slam",
		"SE011 - Gavel Slam Triple",
		0,
		"SE013 - Courtroom Audience 1",
		0,
		0,
		"SE016 - Confirm Save",
		0,
		0,
		"SE019 - Impact",
		"SE01A - Text Typewriter Low",
		0,
		"SE01C - Desk Hit",
		"SE01D - Voice Phoenix Hold It FR",
		"SE01E - Exclamation",
		0,
		"SE020 - Door Open Impact",
		"SE021 - Collapse Impact",
		"SE022 - Lost a Try",
		0,
		"SE024 - Critical Hit",
		"SE025 - Gunshot 1",
		"SE026 - Verbal Hit",
		"SE027 - Voice Phoenix Objection FR",
		"SE028 - Crash into Water",
		"SE029 - Begin Questioning",
		0,
		0,
		"SE02C - Game Over",
		"SE02D - Unexpected Turn of Events",
		0,
		0,
		"SE030 - Phone Ring",
		"SE031 - Phone Pickup",
		0,
		"SE033 - Camera Sound Zoom In",
		0,
		"SE035 - Von Karma Snap",
		
		[102] = "SE03C - Courtroom Audience Victory",
		
		[107] = "SE041 - Phone Ringtone Steel Samurai",
		"SE042 - Short Beep",
		0,
		"SE044 - Unknown",
		"SE045 - Gavel Slam Reverb",
		"SE046 - Whip",
		0,
		0,
		"SE049 - Psyche Lock Slam",
		"SE04A - Psyche Lock Break",
		0,
		"SE04C - Unknown Door Open",
		0,
		0,
		"SE04F - Swoosh",
		"SE050 - Shocking Turn of Events",
		"SE051 - Lion Roar",
		0,
		"SE053 - Spot Light On",
		"SE054 - Unknown Aha",
		0,
		"SE056 - Phyche Lock Chains 1",
		"SE057 - Phyche Lock Chains 2",
		"SE058 - Phyche Lock Chains 3",
		"SE059 - Phyche Lock Chains 4",
		"SE05A - Phyche Lock Chains 5",
		
		[145] = "SE067 - Oldbag Lazerblaster",
		
		[150] = "SE06C - Voice Franziska Objection FR",
		0,
		0,
		"SE06F - Unknown",
		"SE070 - Door Unlock",
		0,
		"SE072 - Life Bar Restore",
		"SE073 - Stomach Growl",
		
		[163] = "SE079 - Courtroom Audience 2",
		"SE07A - Courtroom Audience 3",
		"SE07B - Courtroom Audience Fade",
		"SE07C - Courtroom Headline In",
		"SE07D - Courtroom Headline Out",
		0,
		0,
		0,
		0,
		"SE082 - Radio Detector Low",
		0,
		"SE084 - Radio Detector Medium",
		"SE085 - Unknown Slam",
		0,
		0,
		"SE088 - Coffe Gulp",
		"SE089 - Coffe Swoop",
		"SE08A - Unknown Clap?",
		"SE08B - Coffe Gulp Marathon 2",
		"SE08C - Coffe Gulp Marathon 3",
		"SE08D - Coffe Gulp Marathon 4",
		
		[203] = "BGM053 - Steel Samurais Ballad",
		"BGM054 - Turnabout Sisters Theme 2",
		0,
		0,
		"BGM057 - Kurain Village",
		"BGM058 - Detention Center",
		"BGM059 - Eccentric",
		"BGM060 - Ambient Bird Chirps 3",
		0,
		0,
		0,
		"BGM064 - Investigation Core 2",
		0,
		"BGM066 - Suspense 1",
		0,
		0,
		0,
		0,
		"BGM071 - Trick And Magic",
		0,
		0,
		0,
		0,
		0,
		"BGM077 - Courtroom Lobby 2",
		0,
		"BGM079 - Psyche Lock 3",
		0,
		"BGM081 - Jingle Evidence 1",
		"BGM082 - Franziskas Theme 3",
		0,
		0,
		"BGM085 - Psyche Lock 2",
		"BGM086 - Hotline of Fate",
		"BGM087 - Franziskas Theme 2",
		0,
		0,
		"BGM090 - Audience BGM?",
		0,
		"BGM092 - Pearls Theme",
		
		/* this seems to be a thing */
		[256] = "BGM-SP - ResumePlaying",
		
		[301] = "BGM101 - Intro Case 1",
		"BGM102 - TBD",
		"BGM103 - TBD",
		"BGM104 - Intro Case 5",
		"BGM105 - TBD",
		"BGM106 - TBD",
		"BGM107 - Luke Atmeys Theme",
		"BGM108 - TBD",
		"BGM109 - TBD",
		"BGM110 - TBD",
		"BGM111 - TBD",
		"BGM112 - Hazakura Shrine",
		"BGM113 - Dahlias Theme",
		"BGM114 - TBD",
		"BGM115 - Godots Theme",
		"BGM116 - TBD",
		"BGM117 - TBD",
		"BGM118 - TBD",
		"BGM119 - TBD",
		"BGM120 - TBD",
		"BGM121 - TBD",
		"BGM122 - TBD",
		"BGM123 - Cornered 3",
		"BGM124 - Cornered 3 Variation",
		"BGM125 - TBD",
		"BGM126 - TBD",
		0,
		"BGM128 - TBD",
		"BGM129 - TBD",
		"BGM130 - TBD",
		"BGM131 - TBD",
		"BGM132 - TBD",
		"BGM133 - TBD",
		"BGM134 - TBD",
		"BGM135 - TBD",
		"BGM136 - TBD",
		"BGM137 - TBD",
		"BGM138 - TBD",
		"BGM139 - TBD",
		0,
		0,
		"BGM142 - Cornered 1 Version 3",
		"BGM143 - Cornered 1 Version 3 Variation",
		
		[350] = "SE090 - Jingle Surprise 1",
		"SE091 - Jingle Surprise 2",
		"SE092 - Jingle Surprise 3",
		"SE093 - Jingle Surprise 4",
		"SE094 - Jingle Surprise 5",
		"SE095 - Security Alarm",
		"SE096 - Alarm Turn Off",
		"SE097 - Unknown Confirm",
		"SE098 - Unknown Snap",
		0,
		"SE09A - Elevator Moving",
		"SE09B - Elevator Arrive",
		"SE09C - Urn Breaking",
		"SE09D - Unknown",
		"SE09E - Unknown Confirm 2",
		"SE09F - Unknown Unlock",
		"SE0A0 - Lightning",
		"SE0A1 - Voice Edgeworth Hold It FR",
		"SE0A2 - Voice Edgeworth Take That FR",
		"SE0A3 - Voice Mia Objection FR",
		"SE0A4 - Voice Mia Hold It FR",
		"SE0A5 - Voice Mia Take That FR",
		"SE0A6 - Voice Godot Objection FR",
		"SE0A7 - Unknown Hit",
		0,
		0,
		"SE0AA - Unknown Critical Hit",
		"SE0AB - Jingle Surprise 6",
		
		[382] = "SE0B0 - Unknown Sizzle Long",
		"SE0B1 - Unknown Sizzle Mid",
		"SE0B2 - Unknown Sizzle Short",
		"SE0B3 - Unknown Sizzle Very Long",
		"SE0B4 - Unknown Bleep",
		"SE0B5 - Unknown Engine",
		"SE0B6 - Unknown Impact",
		"SE0B7 - Unknown",
		"SE0B8 - Unknown",
		0,
		0,
		"SE0BB - Phone Ringtone Godot",
		"SE0BC - Hazakura Bell",
		"SE0BD - Unknown Water",
		"SE0BE - Unknown Splash",
		"SE0BF - Voice Phoenix Epic Objection JP",
		"SE0C0 - Epic Objection",
		"SE0C1 - Ghost Scream",
		"SE0C2 - Ghost Exorcism",
		"SE0C3 - Unknown",
		"SE0C4 - Unknown",
		"SE0C5 - Exclamation 3",
		"SE0C6 - Ghost Scream 2",
		0,
		"SE0C8 - Courtroom Audience",
		
		[500] = "SE0D0 - Voice Phoenix Objection DE",
		"SE0D1 - Voice Phoenix Hold It DE",
		"SE0D2 - Voice Payne Objection DE",
		"SE0D3 - Voice Edgeworth Objection DE",
		0,
		"SE0D5 - Voice Phoenix Take That DE",
		"SE0D6 - Voice Franziska Objection DE",
		// anything past here is from a differen sequence bank that none of the usual tools handle
		// and is therefore pure guesswork
		0,
		0,
		0,
		// this assumes that WAVE_SE_ST01 is the first bank
		// and that the sequences appear in order of the wave files
		"SE0DA - Voice Godot Objection DE",
		"SE0DB - Voice Edgeworth Take That DE",
		"SE0DC - Voice Edgeworth Hold It DE",
		"SE0DD - Voice Mia Objection DE",
		"SE0DE - Voice Mia Take That DE",
		"SE0DF - Voice Mia Hold It DE",
	},
	{
		/* apollo to be done */
		0
	}
};

int getOffsetIndex( unsigned int offset, uint32_t *list, unsigned int count ) {
	int i;
	for( i = 0; i < count; i++ ) if( offset == list[i] ) return i;
	return -1;
}

int getMemidxIndex( unsigned int memidx, uint32_t *list, unsigned int count ) {
	int i, sub = (count*4)+4;
	for( i = 0; i < count; i++ ) if( memidx == (list[i]-sub)/2 ) return i;
	return -1;
}

int main( int argc, char **argv ) {
	FILE *f, *o;
	unsigned int fileSize, i, j, gamenum, memidx, textidx, scriptsize, missingargs, isunity = 0;
	uint32_t numScripts, *scriptOffsets = NULL;
	//~ uint16_t token, *arguments = NULL, *scriptfile = NULL;
	uint16_t token, arguments[10] = {0}, *scriptfile = NULL;
	operator curop;
	char textfile[0x100000] = {0}; /* 1M should be enough */
	if( argc < 3 ) {
	printf("Not enough args!\nUse: %s [binary script] [gamenum]\nwhere gamenum is\n1 - original phoenix wright\n2 - justice for all\n3 - trials and tribulations\n4 - apollo justice\nadd 10 to enable unity mode", argv[0]);
		return 1;
	}
	
	gamenum = strtoul(argv[2], NULL, 10) - 1;
	if(gamenum > 9) {
		isunity = 1;
		gamenum -= 10;
	}
	if( gamenum > 3 ) {
		printf("unsupported gamenum %d\n", gamenum+1);
		return 1;
	}
	
	if( !(f = fopen( argv[1], "rb" ))) {
		printf("Couldnt open file %s\n", argv[1]);
		return 1;
	}
	
	char *outfilename = malloc( strlen( argv[1] ) + 4 + 1 );
	
	sprintf( outfilename, "%s.txt", argv[1] );
	if( !(o = fopen( outfilename, "w" ))) {
			printf("Couldnt open file %s\n", outfilename);
			return 1;
	}
	
	fseek( f, 0, SEEK_END );
	fileSize = ftell(f);
	fseek( f, 0, SEEK_SET );
	fread( &numScripts, sizeof(uint32_t), 1, f );
	scriptOffsets = malloc( numScripts * sizeof(uint32_t));
	//~ for( i = 0; i < numScripts; i++ ) fread( &scriptOffsets[i], sizeof(uint32_t), 1, f );
	fread(scriptOffsets, sizeof(uint32_t)*numScripts, 1, f);
	scriptsize = fileSize - ((numScripts*4)+4);
	scriptfile = malloc(scriptsize);
	fread(scriptfile, scriptsize, 1, f);
	memidx = 0;
	
	strcpy(textfile, "<section 0>\n\t");
	textidx = 13;
	while( memidx < scriptsize/2 - 2 ) {
		missingargs = 1;
		if(textidx > (0x100000-100)) {
			printf("converted textfile is approaching the 1M limit, now at 100 or less chars remaining, aborting\n");
			return 1;
		}
		if( getMemidxIndex( memidx, scriptOffsets, numScripts ) > 0 ) {
			sprintf( textfile+textidx, "\n</section>\n<section %03u>\n\t", getMemidxIndex( memidx, scriptOffsets, numScripts));
			textidx += 27;
		}
		token = scriptfile[memidx];
		memidx++;
		if( gamenum < 3 && token > 127 ) {
			if(isunity) {
				token -= (128-32);
				sprintf( textfile+textidx, "%c", (char)token);
				textidx += 1;
			}
			else {
				token -= 128;
				if( (token < sizeofarr(charset)) && (charset[token] != 0) ) {
					sprintf( textfile+textidx, "%s", charset[token]);
					textidx += strlen(charset[token]);
				}
				else {
					sprintf( textfile+textidx, "{%05u}", token+128 );
					textidx += 7;
				}
			}
			//~ if( token > 1 && token < 256 && !(charset[token] == 0) ) {
				//~ sprintf( textfile+textidx, "{%05u}", token+128 );
				//~ textidx += 7;
			//~ }
		}
		else if( gamenum == 3 && token > (127+16) ) {
			token -= (128+16);
			//~ if( token > 1 && token < 256 && !(strcmp(charset[token], "0")) ) {
				//~ sprintf( textfile+textidx, "{%05u}", token+128+16 );
				//~ textidx += 7;
			//~ }
			if( (token < sizeofarr(charset)) && (charset[token] != 0)) {
				sprintf( textfile+textidx, "%s", charset[token]);
				textidx += strlen(charset[token]);
			}
			else {
				sprintf( textfile+textidx, "{%05u}", token+128+16 );
				textidx += 7;
			}
		}
		//~ else if( gamenum == 3 && token > 127 ) {}
		else {
			curop = opcodeList[token];
			if(token > 0x7F) printf("apollo - curtoken %08x\n", token);
			if( curop.args > 0 ) {
				//~ arguments = malloc( curop.args * sizeof(uint16_t));
				for( j = 0; j < curop.args; j++ ) {
					arguments[j] = scriptfile[memidx];
					memidx++;
				}
			}
			//~ if(textfile[textidx-1] != '\t') {
				//~ sprintf(textfile+textidx, "\n\t");
				//~ textidx+=2;
			//~ }
			if( curop.args == 0 ) {
				sprintf( textfile+textidx, "<%s>", curop.name );
				textidx += strlen(curop.name) + 2;
				if(token == 0) {
					sprintf(textfile+textidx, "\n\t");
					textidx+=2;
				}
			}
			else if( curop.args == 1 ) {
				/* color opcode */		if((token == 3) && (arguments[0] < 4)) {
					sprintf( textfile+textidx, "<%s:%s>\n\t", curop.name, colors[arguments[0]] );
					textidx += strlen(curop.name) + strlen(colors[arguments[0]]) + 5;
				}
				/* the bitshift is needed cause capcom seems to store the person in the upper 8 bits of the 16bit argument...
				   removed check if argument 1 is less then 55 because it seems to be ok and apollo exceeds this by 5 :/ */
				/* name opcode */		else if((token == 14) && ((arguments[0] >> 8) < sizeofarr(speakers[gamenum])) && (speakers[gamenum][(arguments[0] >> 8)] != 0)) {
					sprintf( textfile+textidx, "<%s:\"%s\">\n\t", curop.name, speakers[gamenum][(arguments[0] >> 8)] );
					textidx += strlen(curop.name) + strlen(speakers[gamenum][(arguments[0] >> 8)]) + 7;
				}
				/* background opcode */		else if((token == 27) && (arguments[0] < sizeofarr(backgrounds[gamenum])) && (backgrounds[gamenum][arguments[0]] != 0)) {
					sprintf( textfile+textidx, "<%s:\"%s\">\n\t", curop.name, backgrounds[gamenum][arguments[0]] );
					textidx += strlen(curop.name) + strlen(backgrounds[gamenum][arguments[0]]) + 7;
				}
				/* rejmp and jmp opcode */	else if((token == 10) || (token == 44)) {
					sprintf( textfile+textidx, "<%s:%05u>\n\t", curop.name, arguments[0]-128 );
					textidx += strlen(curop.name) + 10;
				}
				/* shift_background opcode */	else if(token == 29) {
					sprintf( textfile+textidx, "<%s:%s - %04u>\n\t", curop.name, shiftdirection[arguments[0]/256], arguments[0] % 256 );
					textidx += strlen(curop.name) ;
					textidx += strlen(shiftdirection[arguments[0]/256]) + 12;
				}
				else {
					sprintf( textfile+textidx, "<%s:%05u>\n\t", curop.name, arguments[0] );
					textidx += strlen(curop.name) + 10;
				}
			}
			else {
				/* removed check if argument 1 is less then 55 because it seems to be ok and apollo exceeds this by 5 :/ */
				/* readded check because it broke things for phoenix 1 */
				/* person opcode */		if((token == 30) && (arguments[0] < sizeofarr(speakers[gamenum])) && (speakers[gamenum][arguments[0]] != 0)) {
					sprintf( textfile+textidx, "<%s:\"%s\"", curop.name, speakers[gamenum][arguments[0]] );
					textidx += strlen(curop.name) + strlen(speakers[gamenum][arguments[0]]) + 4;
				}
				/* fademusic opcode */		else if((token == 34) && (arguments[0] < sizeofarr(musicfading))) {
					sprintf( textfile+textidx, "<%s:\"%s\"", curop.name, musicfading[arguments[0]] );
					textidx += strlen(curop.name) + strlen(musicfading[arguments[0]]) + 4;
				}
				/* 2 choice jmp opcode */	else if(token == 8) {
					arguments[0] -= 128;
					arguments[1] -= 128;
					sprintf( textfile+textidx, "<%s:%05u", curop.name, arguments[0] );
					textidx += strlen(curop.name) + 7;
				}
				/* 3 choice jmp opcode */	else if(token == 9) {
					arguments[0] -= 128;
					arguments[1] -= 128;
					arguments[2] -= 128;
					sprintf( textfile+textidx, "<%s:%05u", curop.name, arguments[0] );
					textidx += strlen(curop.name) + 7;
				}
				/* music and sound opcode */	else if(((token == 5) || (token == 6)) && (arguments[0] < sizeofarr(sound_data[gamenum])) && (sound_data[gamenum][arguments[0]] != 0)) {
					sprintf( textfile+textidx, "<%s:\"%s\"", curop.name, sound_data[gamenum][arguments[0]] );
					textidx += strlen(curop.name) + strlen(sound_data[gamenum][arguments[0]]) + 4;
				}
				/* animation opcode */		else if((token == 47) && (arguments[1] < sizeofarr(animationstate))) {
					sprintf( textfile+textidx, "<%s:%05u:\"%s\"", curop.name, arguments[0], animationstate[arguments[1]] );
					textidx += strlen(curop.name) + 10 + strlen(animationstate[arguments[1]]);
					missingargs = 0;
				}
				else {
					sprintf( textfile+textidx, "<%s:%05u", curop.name, arguments[0] );
					textidx += strlen(curop.name) + 7;
				}
				if(missingargs) {
					for( j = 1; j < curop.args; j++ ) {
						sprintf( textfile+textidx, ",%05u", arguments[j] );
						textidx += 6;
					}
				}
				sprintf( textfile+textidx, ">\n\t" );
				textidx += 3;
			}
			//~ if( arguments ) {
				//~ free(arguments);
				//~ arguments = 0;
			//~ }
		}
	}
	fwrite(textfile, textidx, 1, o);
	fclose(f);
	fclose(o);
	free(outfilename);
	free(scriptOffsets);
	free(scriptfile);
	return 0;
}
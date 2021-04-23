#ifndef MACROS_H_
#define MACROS_H_

enum supportedgames {
	GAME_PHOENIX1,
	GAME_PHOENIX2,
	GAME_PHOENIX3,
	GAME_APOLLO,
	GAME_GS1GBA,
	GAME_NUMGAMES
};

#define ISNDS(x) ((x) >= GAME_PHOENIX1 && (x) < GAME_GS1GBA)
#define ISGBA(x) ((x) >= GAME_GS1GBA && (x) < GAME_NUMGAMES)

// 0x90
#define NUMCOMMANDS 144

#define ARRGAMENUM(x) ((x) % 4)
#define sizeofarr(a) (sizeof(a) / sizeof(a[0]))

#endif
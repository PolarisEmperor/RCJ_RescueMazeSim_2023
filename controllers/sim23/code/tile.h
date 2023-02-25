#pragma once

#include "lib.h"
#include "bot.h"

typedef unsigned char ubyte;

// Old struct
//struct Tile {
//	union {
//		struct {
//			ubyte N : 1; // north wall
//			ubyte E : 1; // east wall
//			ubyte S : 1; // south wall
//			ubyte W : 1; // west wall
//			ubyte visited : 1; // has robot visited tile already
//			ubyte code : 4; // hsu, hazards
//		};
//		ubyte bits;
//	};
//};

struct Tile {
	union {
		struct {
			ubyte N : 1; // north wall
			ubyte E : 1; // east wall
			ubyte S : 1; // south wall
			ubyte W : 1; // west wall
			ubyte visited : 1; // has robot visited tile already
			ubyte code : 3; // idk what we're using this for yet
		};
		ubyte bits;
	};
};

//const int ROWS = 40;
//const int COLS = 40;
//const int fieldSize = ROWS * COLS;
extern Tile field[fieldSize];
//extern int room;

void getTile(int tile);
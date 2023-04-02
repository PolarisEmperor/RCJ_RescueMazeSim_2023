#pragma once

#include "lib.h"
#include "bot.h"

typedef unsigned char ubyte;

struct Tile {
	union {
		struct {
			ubyte N : 1; // north wall
			ubyte E : 1; // east wall
			ubyte S : 1; // south wall
			ubyte W : 1; // west wall
		};
		ubyte bits;
	};
	bool visited; // has robot visited tile already
	bool victimChecked; // victim seen
};

extern Tile field[fieldSize];

void setWalls(int tile, bool N, bool E, bool S, bool W);
void getTile(int tile);
//unsigned char moveBits(unsigned char bits, int n); // temp, remove when done
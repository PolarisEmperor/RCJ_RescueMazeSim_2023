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
			ubyte tl : 1; // curve between W and N 
			ubyte tr : 1; // curve between N and E
			ubyte br : 1; // curve between E and S
			ubyte bl : 1; // curve between S and W
		};
		ubyte bits;
	};
	bool visited; // has robot visited tile already
};

extern Tile field[fieldSize];

void getTile(int tile);
unsigned char moveBits(unsigned char bits, int n); // temp, remove when done
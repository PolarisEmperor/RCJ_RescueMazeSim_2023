#pragma once

#include <string>
#include "lib.h"
#include "bot.h"

enum TileColor { Normal, Hole = 2, Swamp, Checkpoint, Blue = 6, Purple, Red, Green };
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
	unsigned char color; // Tile color
};

extern Tile field[fieldSize];

void setWalls(int tile, bool N, bool E, bool S, bool W);
void getTile(int tile);
void sendMap();
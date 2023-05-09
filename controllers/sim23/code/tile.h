#pragma once

#include <string>
#include <vector>
#include <numeric>
#include "lib.h"
#include "bot.h"

enum TileColor { Normal, Hole = 2, Swamp, Checkpoint, Start, Blue, Purple, Red, Green };
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
	int corner;
	bool visited; // has robot visited tile already
	bool victimChecked; // victim seen
	char color; // Tile color
};

extern Tile field[fieldSize];
extern std::vector<std::vector<std::string>> Map;
extern char bigmap[3 * ROWS + ROWS + 1][3 * ROWS + ROWS + 1];


void setWalls(int tile, bool N, bool E, bool S, bool W);
void getTile(int tile);
void mapBonus();
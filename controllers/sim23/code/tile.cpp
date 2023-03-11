#include "tile.h"

Tile field[fieldSize];

// Shift NESW wall bits in Tile structure to the left
// if n == 1, nesw == 0110 will become 1100
// and 1001 will become 0011, the highest order bit will move down to the lowest order bit.
unsigned char moveBits(unsigned char bits, int n) {
	ubyte walls = bits & 0x0f;  // 0000xxxx
	ubyte mask;
	walls <<= n;

	mask = walls & 0xf0;
	walls &= ~mask;
	walls |= mask >> 4;

	bits &= ~0x0f;
	bits |= walls;

	return bits;
}

// Get tile data(set bits of walls)
void getTile(int tile) {
	int directions[4] = { tile, tile + 1, tile + COLS + 1, tile + COLS };
	int outer[] = { tile - COLS, tile - COLS + 1, tile + 2, tile + 2 + COLS, tile + COLS + COLS, tile + COLS + COLS + 1, tile - 1, tile - 1 + COLS };
	int n = 30;
	int check[] = { 511-n, 0+n, 127-n, 127+n, 255-n, 255+n, 383-n, 383+n }; // lidar values to check
	int dir = bot.getDirection();
	int bit = 1;
	
	// clear structure
	for (int i = 0; i < 4; i++) {
		field[directions[i]].N = field[directions[i]].E = field[directions[i]].S = field[directions[i]].W = 0;
	}

	// Scan for walls with Lidar
	for (int i = 0; i < sizeof(check) / sizeof(check[0]); i++) {
		if (bot.getLidar(3, check[i]) < 7)
			field[directions[dir]].bits += bit; // set wall bit

		// odd:        next wall  next tile
		(i % 2 != 0) ? bit *= 2 : (dir == West) ? dir = North : dir++;
	}

	// Outer neighbors / side half walls
	if (bot.curRoom == 2) {
		printf("CURDIR %d\n", bot.getDirection());
		for (int i = North; i <= West; i++) {
			for (int j = i * 128 - 30; j < i * 128 + 30; j++) {
				if (bot.getLidar(3, i * 128 - 30) < 7 || bot.getLidar(3, i * 128 + 30) < 7) break;
				if (bot.getLidar(3, j) < 7) {
					int wall = i + dir;
					if (wall >= 4) wall -= 4;
					field[outer[wall * 2]].bits |= (wall % 2 == 0) ? 2 : 4;
					field[outer[wall * 2 + 1]].bits |= (wall % 2 == 0) ? 8 : 1;
					printf("mid wall thingy  dir %d\n", i);
					printf("WALL %d\n", wall);
					printf("tile %d %d\n", outer[wall * 2], (wall % 2 == 0) ? 2 : 4);
					printf("tile %d %d\n", outer[wall* 2 + 1], (wall % 2 == 0) ? 8 : 1);
					bot.delay(1000);
					break;
				}
			}
		}
	}

	// fix direction
	for (int i = 0; i < 4; i++)
		field[directions[i]].bits = moveBits(field[directions[i]].bits, bot.getDirection());

	// set opposite wall bits for neighbor nodes for all 4 tiles
	for (int i = 0; i < 4; i++) {
		int t = directions[i];
		int neighbors[] = { t - COLS, t + 1, t + COLS, t - 1 };
		for (int j = North; j <= West; j++) {
			switch (j) {
				// error checking
				case North: if (t < COLS) continue;
				case East:  if (t % COLS == 0) continue;
				case South: if (t >= COLS * (ROWS - 1)) continue;
				case West:  if ((t + 1) % COLS == 0) continue;
			}
			if (field[t].bits & (1 << j)) // check bit
				field[neighbors[j]].bits |= moveBits(1 << j, 2); // set bit
		}
	}

	printf("TILE %d\n", tile);
	for (int i = 0; i < 4; i++)
		printf("%d\n", field[directions[i]].bits);

	field[directions[0]].visited = 1;

	//if (bot.curRoom == 1)
	//	for (int i = 1; i < sizeof(directions) / sizeof(directions[0]); i++)
	//		field[directions[i]].visited = 1;
}

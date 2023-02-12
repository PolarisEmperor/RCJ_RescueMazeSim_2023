#include "tile.h"

Tile field[fieldSize];

// Shift NESW wall bits in Tile structure to the left
// if n == 1, nesw == 0110 will become 1100
// and 1001 will become 0011, the highest order bit will move down to the lowest order bit.
unsigned char moveBits(unsigned char bits, int n) {
	if (!n) return bits;

	bool HOB = bits & (1 << 3); // is highest order bit set
	bool visited = bits & (1 << 4); // is visited bit set
	bits <<= 1; // left shift
	if (!visited) bits &= ~(1 << 4); // clear shifted bit
	if (HOB) bits |= 1; // HOB is now LOB

	return moveBits(bits, n - 1);
}

// Get tile data(set bits of walls)
void getTile(Bot *bot, unsigned int tile) {
	unsigned int directions[4] = { tile, tile + 1, tile + COLS + 1, tile + COLS };
	int check[] = { 496, 15, 112, 142, 240, 270, 368, 398 }; // lidar values to check
	int dir = bot->getDirection();
	int bit = 1;
	
	// clear structure
	for (int i = 0; i < 4; i++) {
		field[directions[i]].N = field[directions[i]].E = field[directions[i]].S = field[directions[i]].W = 0;
	}

	for (int i = 0; i < sizeof(check) / sizeof(check[0]); i++) {
		// set wall bit
		if (bot->getLidar(3, check[i]) < 7)
			field[directions[dir]].bits += bit; 

		// odd:        next wall  next tile
		(i % 2 != 0) ? bit *= 2 : (dir == West) ? dir = North : dir++; 
	}
	
	// fix direction
	for (int i = 0; i < 4; i++)
		field[directions[i]].bits = moveBits(field[directions[i]].bits, bot->getDirection());

	// set opposite wall bits for neighbor nodes for all 4 tiles
	for (int i = 0; i < 4; i++) {
		int t = directions[i];
		unsigned int neighbors[] = { t - COLS, t + 1, t + COLS, t - 1 };
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

	printf("TILE\n");
	for (int i = 0; i < 4; i++)
		printf("%d\n", field[directions[i]].bits);

	field[tile].visited = 1;
	if (room == 1)
		for (int i = 1; i < sizeof(directions) / sizeof(directions[0]); i++)
			field[directions[i]].visited = 1;
}
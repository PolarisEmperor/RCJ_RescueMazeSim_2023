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
void getTile(Bot *bot, int tile) {
	int directions[4] = { tile, tile + 1, tile + COLS + 1, tile + COLS };
	int outer[] = { tile - COLS, tile - COLS + 1, tile + 2, tile + 2 + COLS, tile + COLS + COLS + 1, tile + COLS + COLS, tile - 1 + COLS, tile - 1 };
	int check[] = { 496, 15, 112, 142, 240, 270, 368, 398 }; // lidar values to check
	int dir = bot->getDirection();
	int bit = 1;

	// clear structure
	for (int i = 0; i < 4; i++) {
		field[directions[i]].N = field[directions[i]].E = field[directions[i]].S = field[directions[i]].W = 0;
	}
	if (room == 2)
		for (int i = 0; i < sizeof(outer) / sizeof(outer[0]); i++)
			if (field[outer[i]].visited == 0)
				field[outer[i]].N = field[outer[i]].E = field[outer[i]].S = field[outer[i]].W = 0;

	// Scan for walls with Lidar
	for (int i = 0; i < sizeof(check) / sizeof(check[0]); i++) {
		if (bot->getLidar(3, check[i]) < 7)
			field[directions[dir]].bits += bit; // set wall bit

		// odd:        next wall  next tile
		(i % 2 != 0) ? bit *= 2 : (dir == West) ? dir = North : dir++;
	}

	// Outer neighbors / side half walls
	if (room == 2) {
		for (int i = North; i <= West; i++) {
			if (bot->getLidar(3, i * 127) < 7) {
				// error checking
				if (outer[i * 2] < 0 || outer[i * 2] % COLS == 0 || outer[i * 2] >= COLS * (ROWS - 1) || (outer[i * 2] + 1) % COLS == 0)
					break;

				// no walls
				if ((field[outer[i * 2]].bits & moveBits((1 << i), 2)) == 1 ||
					(field[outer[i * 2 + 1]].bits & moveBits((1 << i), 2)) == 1)
					break;

				field[outer[i * 2]].bits |= (i == West) ? moveBits(1 << (i - 1), 2) : 1 << (i + 1);
				field[outer[i * 2 + 1]].bits |= (i == West) ? 1 << (i - 1) : moveBits(1 << (i + 1), 2);
				printf("mid wall thingy  dir %d\n", i);
				printf("outer bits %d %d\n", field[outer[i * 2]].bits, field[outer[i * 2 + 1]].bits);

				bot->delay(3000);

			}
		}
	}
	// fix direction
	for (int i = 0; i < 4; i++)
		field[directions[i]].bits = moveBits(field[directions[i]].bits, bot->getDirection());

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

	printf("TILE\n");
	for (int i = 0; i < 4; i++)
		printf("%d\n", field[directions[i]].bits);

	field[tile].visited = 1;
	if (room == 1)
		for (int i = 1; i < sizeof(directions) / sizeof(directions[0]); i++)
			field[directions[i]].visited = 1;
}

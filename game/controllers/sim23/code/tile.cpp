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
	
	//unsigned int directions[] = { tile - COLS, tile + 1, tile + COLS, tile - 1 };
	unsigned int directions[] = { tile, tile + 1, tile + COLS + 1, tile + COLS };
	int check[] = { 496, 15, 112, 142, 240, 270, 368, 398 }; // lidar values to check
	int dir = bot->getDirection();
	int bit = 1;
	
	
	// clear structure
	for (int i = 0; i < sizeof(directions) / sizeof(directions[0]); i++) {
		field[directions[i]].N = field[directions[i]].E = field[directions[i]].S = field[directions[i]].W = 0;
	}

	//// check 4 walls **OLD**
	//for (int i = North; i <= West; i++) {
	//	if (bot->getLidar(2, bot->getLidarRes() / 4 * i) < cm) {
	//		field[tile].bits |= (1 << i); // set bit of wall
	//	}
	//}

	for (int i = 0; i < sizeof(check) / sizeof(check[0]); i++) {
		if (bot->getLidar(3, check[i]) < 6) {
			field[directions[dir]].bits += bit; // set wall bit
		}

		if (i % 2 != 0) { // odd, next wall
			bit *= 2;
		}
		else {
			(dir == West) ? dir = North : dir++; // next tile
		}
	}

	
	/*if (bot->getLidar(3, 496) < 10) {
		field[directions[North]].N = 1;
		printf("top left\n");
	}
	if (bot->getLidar(3, 15) < 10) {
		field[directions[East]].N = 1;
		printf("top right\n");
	}
	if (bot->getLidar(3, 112) < 10) {
		field[directions[East]].E = 1;
		printf("right top\n");
	}
	if (bot->getLidar(3, 142) < 10) {
		field[directions[South]].E = 1;
		printf("right bot\n");
	}
	if (bot->getLidar(3, 240) < 10) {
		field[directions[South]].S = 1;
		printf("bot right\n");
	}
	if (bot->getLidar(3, 270) < 10) {
		field[directions[West]].S = 1;
		printf("bot left\n");
	}
	if (bot->getLidar(3, 368) < 10) {
		field[directions[West]].W = 1;
		printf("left bot\n");
	}
	if (bot->getLidar(3, 398) < 10) {
		field[directions[North]].W = 1;
		printf("left top\n");
	}*/

	
	// fix direction
	for (int i = 0; i < sizeof(directions) / sizeof(directions[0]); i++) {
		field[directions[i]].bits = moveBits(field[directions[i]].bits, bot->getDirection());
	}

	// set opposite wall bits for neighbor nodes
	//for (int i = North; i <= West; i++) {
	//	switch (i) {
	//		// error checking
	//		case North: if (tile < COLS) continue;
	//		case East:  if (tile == 0) continue;
	//		case South: if (tile >= COLS * (ROWS - 1)) continue;
	//		case West:  if (tile == ROWS * COLS - 1) continue;
	//	}

	//	if (field[tile].bits & (1 << i)) { // check bit
	//		field[directions[i]].bits |= moveBits(1 << i, 2); // set bit
	//	}
	//}
	
	//// fix direction
	//switch (bot->getDirection()) {
	//case East:  field[tile].bits = moveBits(field[tile].bits, 1); break;
	//case South: field[tile].bits = moveBits(field[tile].bits, 2); break;
	//case West:  field[tile].bits = moveBits(field[tile].bits, 3); break;
	//}

	// set opposite wall bits for neighbor nodes for all 4 tiles
	for (int i = 0; i < 4; i++) {
		int t = directions[i];
		unsigned int neighbors[] = { t - COLS, t + 1, t + COLS, t - 1 };
		//printf("t = %d neighbors = %d %d %d %d\n", t, neighbors[0], neighbors[1], neighbors[2], neighbors[3]);
		//printf("BITS = %d\n", field[t].bits);
		for (int j = North; j <= West; j++) {
			switch (j) {
			// error checking
				case North: if (t < COLS) continue;
				case East:  if (t % COLS == 0) continue;
				case South: if (t >= COLS * (ROWS - 1)) continue;
				case West:  if ((t + 1) % COLS == 0) continue;
			}
			if (field[t].bits & (1 << j)) { // check bit

				//printf("setting bit\n");
				field[neighbors[j]].bits |= moveBits(1 << j, 2); // set bit
				//printf("%d %d\n", i, j);
				//printf("neighbor = %d bit = %d\n", neighbors[j], field[neighbors[j]].bits);
			}
		}
	}
	
	//printf("top left: %d\n", field[directions[North]].bits);
	//printf("top right: %d\n", field[directions[East]].bits);
	//printf("bottom right: %d\n", field[directions[South]].bits);
	//printf("bottom left: %d\n", field[directions[West]].bits);

	//for (int i = North; i <= West; i++) {
	//	switch (i) {
	//	// error checking
	//	case North: if (tile < COLS - 1) continue;
	//	case East:  if (tile == 0) continue;
	//	case South: if (tile >= COLS * (ROWS - 1)) continue;
	//	case West:  if (tile == ROWS * COLS - 1) continue;
	//	}
	// 
	//	if (field[tile].bits & (1 << i)) { // check bit
	//		field[directions[i]].bits |= moveBits(1 << i, 2); // set bit
	//	}
	//}

	//printf("bits = %d\n", field[tile].bits);

	field[tile].visited = 1;
}

// Check if valid neighbor
int findNeighbor(unsigned int tile, int dir) {
	unsigned int neighbors[] = { tile - COLS, tile + 1, tile + COLS, tile - 1 }; // tiles to the north, east, south, west

	//if ((field[tile].bits & (1 << dir)) == 0) { // check that there is no wall on the respective side of the current node
	//	return neighbors[dir]; // return neighbor
	//}
	
	switch (dir) {
		case North:
			if ((field[tile].bits & (1 << North)) == 0 && (field[neighbors[East]].bits & (1 << North)) == 0 && (field[neighbors[North]].bits & (1 << East)) == 0) {
				return neighbors[dir];
			}
			break;
		case East:
			if ((field[neighbors[East]].bits & (1 << East)) == 0 && (field[neighbors[South] + 1].bits & (1 << East)) == 0 && (field[neighbors[East] + 1].bits & (1 << South)) == 0) {
				return neighbors[dir];
			}
			break;
		case South:
			if ((field[neighbors[South]].bits & (1 << South)) == 0 && (field[neighbors[South] + 1].bits & (1 << South)) == 0 && (field[neighbors[South] + COLS].bits & (1 << East)) == 0) {
				return neighbors[dir];
			}
			break;
		case West:
			if ((field[tile].bits & (1 << West)) == 0 && (field[neighbors[South]].bits & (1 << West)) == 0 && (field[neighbors[West]].bits & (1 << South)) == 0) {
				return neighbors[dir];
			}
			break;
	}
	return -1; // Tile is not a neighbor
}
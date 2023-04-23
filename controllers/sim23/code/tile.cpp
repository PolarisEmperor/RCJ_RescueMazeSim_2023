#include "tile.h"

using namespace std;

Tile field[fieldSize];

void setWalls(int tile, bool N, bool E, bool S, bool W) {
	// tiles to the north, east, south, west directions
	int neighbors[4] = { tile - COLS, tile + 1, tile + COLS, tile - 1 };
	
	if (N) {
		field[tile].N = 1;
		field[neighbors[North]].S = 1;
	}
	if (E) {
		field[tile].E = 1;
		field[neighbors[East]].W = 1;
	}
	if (S) {
		field[tile].S = 1;
		field[neighbors[South]].N = 1;
	}
	if (W) {
		field[tile].W = 1;
		field[neighbors[West]].E = 1;
	}
}

// Shift NESW wall bits in Tile structure to the left
// if n == 1, nesw == 0110 will become 1100
// and 1001 will become 0011, the highest order bit will move down to the lowest order bit.
unsigned char moveBits(unsigned char bits, int n) {
	ubyte walls = bits & 0x0f;  // 0000xxxx
	ubyte mask;

	walls <<= n; // left shift
	mask = walls & 0xf0; // set mask
	walls &= ~mask; // clear mask
	walls |= mask >> 4; // set pushed-over bits
	bits &= ~0x0f; // clear wall bits
	bits |= walls; // set new wall bits

	return bits;
}

// Get tile data(set bits of walls)
void getTile(int tile) {
	int directions[4] = { tile, tile + 1, tile + COLS + 1, tile + COLS }; // sub-tiles
	int outer[] = { tile - COLS, tile - COLS + 1, tile + 2, tile + 2 + COLS, tile + COLS + COLS, tile + COLS + COLS + 1, tile - 1, tile - 1 + COLS }; // neighbors of sub-tiles
	int n = 30; // lidar range
	int check[] = { 511-n, 0+n, 127-n, 127+n, 255-n, 255+n, 383-n, 383+n }; // lidar values to check
	int dir = bot.getDirection();
	int bit = 1;
	int distFromWall = 9;
	bool wall[8] = { 0 };

	if (field[tile].visited) return;
	// clear structure
	for (int i = 0; i < 4; i++) {
		field[directions[i]].N = field[directions[i]].E = field[directions[i]].S = field[directions[i]].W = 0;
	}

	// Scan for walls with Lidar
	for (int i = 0; i < sizeof(check) / sizeof(check[0]); i++) {
		if (bot.getLidar(3, check[i]) < distFromWall) {
			if (i + bot.getDirection() * 2 > 7) {
				wall[i + bot.getDirection() * 2 - 8] = true;
			}
			else {
				wall[i + bot.getDirection() * 2] = true;
			}
			printf("wall dir %d bot dir %d real dir %d\n", i, bot.getDirection(), (i + bot.getDirection() * 2 > 7) ? (i + bot.getDirection() * 2) - 8 : i + bot.getDirection() * 2);
			//field[directions[dir]].bits += bit; // set wall bit
		}
		// odd:        next wall  next tile
		(i % 2 != 0) ? bit *= 2 : (dir == West) ? dir = North : dir++;
	}

	for (int i = 0; i < 8; i++) {
		if (wall[i]) {
			switch (i) {
				case 0:
					setWalls(tile, 1, 0, 0, 0);
					break;
				case 1:
					setWalls(tile + 1, 1, 0, 0, 0);
					break;
				case 2:
					setWalls(tile + 1, 0, 1, 0, 0);
					break;
				case 3:
					setWalls(tile + COLS + 1, 0, 1, 0, 0);
					break;
				case 4:
					setWalls(tile + COLS + 1, 0, 0, 1, 0);
					break;
				case 5:
					setWalls(tile + COLS, 0, 0, 1, 0);
					break;
				case 6:
					setWalls(tile + COLS, 0, 0, 0, 1);
					break;
				case 7:
					setWalls(tile, 0, 0, 0, 1);
					break;
			}
		}
	}

	// Check for side half walls
	//if (bot.curRoom == 2) {
		//printf("CURDIR %d\n", bot.getDirection());
		for (int i = North; i <= West; i++) {
			for (int j = i * 128 - n; j < i * 128 + n; j++) {
				if (bot.getLidar(3, i * 128 - 30) < 7 || bot.getLidar(3, i * 128 + 30) < 7) break;
				if (bot.getLidar(3, j) < 7) {
					int wall = i + dir; // what side the wall is on
					if (wall >= 4) wall -= 4; // fix side

					// set bits
					field[outer[wall * 2]].bits |= (wall % 2 == 0) ? 2 : 4;
					field[outer[wall * 2 + 1]].bits |= (wall % 2 == 0) ? 8 : 1;

					printf("mid wall thingy  dir %d\n", i);
					printf("WALL %d\n", wall);
					printf("tile %d %d\n", outer[wall * 2], (wall % 2 == 0) ? 2 : 4);
					printf("tile %d %d\n", outer[wall* 2 + 1], (wall % 2 == 0) ? 8 : 1);
					break;
				}
			}
		}
	//}

	//// fix direction
	//for (int i = 0; i < 4; i++)
	//	field[directions[i]].bits = moveBits(field[directions[i]].bits, bot.getDirection());

	//// set opposite wall bits for neighbor nodes for all 4 tiles
	//for (int i = 0; i < 4; i++) {
	//	int t = directions[i];
	//	int neighbors[] = { t - COLS, t + 1, t + COLS, t - 1 };
	//	for (int j = North; j <= West; j++) {
	//		switch (j) {
	//			// error checking
	//			case North: if (t < COLS) continue;
	//			case East:  if (t % COLS == 0) continue;
	//			case South: if (t >= COLS * (ROWS - 1)) continue;
	//			case West:  if ((t + 1) % COLS == 0) continue;
	//		}
	//		if (field[t].bits & (1 << j)) // check bit
	//			field[neighbors[j]].bits |= moveBits(1 << j, 2); // set bit
	//	}
	//}

	printf("TILE %d\n", tile);
	for (int i = 0; i < 4; i++)
		printf("%d\n", field[directions[i]].bits);

	field[directions[0]].visited = 1;
}

bool isEmptyRow(int row) {
	// Loop through all cols in row
	for (int col = 0; col < COLS; col++) {
		if (field[row * ROWS + col].visited) return false;
	}
	return true;
}

bool isEmptyCol(int col) {
	// Loop through all rows in col
	for (int row = 0; row < ROWS; row++) {
		if (field[row * ROWS + col].visited) return false;
	}
	return true;
}

void createMap() {
	unsigned int width, height, startX = 0, startY = 0, endX, endY, rows, cols;

	// Calculate Height and Width of map
	while (isEmptyCol(startX)) startX++;
	endX = startX;
	while (!isEmptyCol(endX)) endX++;
	height = (endX - startX + 1) * 2 + 1;

	while (isEmptyRow(startY)) startY++;
	endY = startY;
	while (!isEmptyRow(endY)) endY++;

	width = (endY - startY + 1) * 2 + 1;
	rows = (endY - startY) / 2 + 1;
	cols = (endX - startX) / 2 + 1;

	printf("Height = %d\n", height);
	printf("Width = %d\n", width);
	printf("Rows = %d\nCols = %d\n", rows, cols);
	vector<vector<string>> map(width, vector<string>(height, string("")));
	printf("%d %d\n", width * height, map.size());
	
	// Find out where start tile is
	

	for (int i = 0; i < fieldSize; i++) {
		if (field[i].visited) {
			if (i == bot.startTile) {
				printf("start tile\n");
			}
			printf("%d ", i);
		}
	}
	printf("\n");
}

void sendMap() {
	createMap();
	const unsigned int width = 9, height = 9;
	string map[width][height] = {
		{ "1", "1", "1", "1", "1", "H", "1", "1", "1" },
		{ "1", "5", "0", "5", "0", "0", "0", "0", "1" },
		{ "1", "0", "0", "0", "0", "0", "0", "0", "1" },
		{ "1", "5", "0", "5", "0", "0", "0", "0", "1" },
		{ "1", "0", "0", "0", "0", "0", "0", "0", "1" },
		{ "1", "0", "0", "0", "0", "0", "0", "0", "S" },
		{ "1", "0", "0", "0", "0", "0", "0", "0", "1" },
		{ "1", "0", "0", "0", "0", "0", "0", "0", "1" },
		{ "1", "U", "1", "1", "1", "1", "1", "1", "1" }
	};

	string flattened = "";
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			printf("%s ", map[i][j]);
			flattened += map[i][j] + ","; // Flatten the array with comma separators
		}
		printf("\n");
	}

	flattened.pop_back(); // Remove the last unnecessary comma

	char *message = (char *)malloc(8 + flattened.size());
	// The first 2 integers in the message array are width, height
	memcpy(message, &width, sizeof(width));
	memcpy(&message[4], &height, sizeof(height));
	memcpy(&message[8], flattened.c_str(), flattened.size()); // Copy in the flattened map afterwards
	bot.emitter->send(message, 8 + flattened.size()); // Send map data
	char msg = 'M'; // Send map evaluate request
	bot.emitter->send(&msg, sizeof(msg));
}
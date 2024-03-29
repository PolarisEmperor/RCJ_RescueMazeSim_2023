#include "tile.h"

using namespace std;

// Shape of curve. Shape looks like a quarter circle
enum cornerDirection { tl, tr, br, bl };

string bigmap[3 * ROWS + ROWS + 1][3 * COLS + COLS + 1];
int mapX = (bot.curTile % COLS) * 2 + 2, mapY = (bot.curTile / ROWS) * 2 + 2;
volatile Tile field[fieldSize];

void updateMapCoords(int tile) {
	mapX = (tile % COLS) * 2 + 2;
	mapY = (tile / ROWS) * 2 + 2;
}

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

// Map out walls
void editMapTile(int tile) {
	updateMapCoords(tile);
	bigmap[mapY][mapX] = "0";
	bigmap[mapY][mapX-1] = "0";
	bigmap[mapY][mapX+1] = "0";
	bigmap[mapY-1][mapX] = "0";
	bigmap[mapY+1][mapX] = "0";

	// North
	if (field[tile].N) {
		if (bigmap[mapY - 2][mapX - 1] == "0") bigmap[mapY - 2][mapX - 1] = "1";
		if (bigmap[mapY - 2][mapX] != "!") bigmap[mapY - 2][mapX] = "1";
	}
	else {
		bigmap[mapY - 2][mapX - 1] = "0";
	}
	if (field[tile + 1].N) {
		if (bigmap[mapY - 2][mapX + 1] == "0") bigmap[mapY - 2][mapX + 1] = "1";
		if (bigmap[mapY - 2][mapX] != "!") bigmap[mapY - 2][mapX] = "1";
	}
	else {
		bigmap[mapY - 2][mapX + 1] = "0";
	}
	if (!field[tile].N && !field[tile + 1].N && field[tile - COLS].E) {
		bigmap[mapY - 2][mapX] = "1";
		bigmap[mapY - 3][mapX] = "1";
	}
	// East
	if (field[tile + 1].E) {
		if (bigmap[mapY - 1][mapX + 2] == "0") bigmap[mapY - 1][mapX + 2] = "1";
		if (bigmap[mapY][mapX + 2] != "!") bigmap[mapY][mapX + 2] = "1";
	}
	else {
		bigmap[mapY - 1][mapX + 2] = "0";
	}
	if (field[tile + COLS + 1].E) {
		if (bigmap[mapY + 1][mapX + 2] == "0") bigmap[mapY + 1][mapX + 2] = "1";
		if (bigmap[mapY][mapX + 2] != "!") bigmap[mapY][mapX + 2] = "1";
	}
	else {
		bigmap[mapY + 1][mapX + 2] = "0";
	}
	if (!field[tile + 1].E && !field[tile + COLS + 1].E && field[tile + 2].S) {
		bigmap[mapY][mapX + 2] = "1";
		bigmap[mapY][mapX + 3] = "1";
	}
	// South
	if (field[tile + COLS].S) {
		if (bigmap[mapY + 2][mapX - 1] == "0") bigmap[mapY + 2][mapX - 1] = "1";
		if (bigmap[mapY + 2][mapX] != "!") bigmap[mapY + 2][mapX] = "1";
	}
	else {
		bigmap[mapY + 2][mapX - 1] = "0";
	}
	if (field[tile + COLS + 1].S) {
		if (bigmap[mapY + 2][mapX + 1] == "0") bigmap[mapY + 2][mapX + 1] = "1";
		if (bigmap[mapY + 2][mapX] != "!") bigmap[mapY + 2][mapX] = "1";
	}
	else {
		bigmap[mapY + 2][mapX + 1] = "0";
	}
	if (!field[tile + COLS].S && !field[tile + COLS + 1].S && field[tile + COLS * 2].E) {
		bigmap[mapY + 2][mapX] = "1";
		bigmap[mapY + 3][mapX] = "1";
	}
	// West
	if (field[tile].W) {
		if (bigmap[mapY - 1][mapX - 2] == "0") bigmap[mapY - 1][mapX - 2] = "1";
		if (bigmap[mapY][mapX - 2] != "!") bigmap[mapY][mapX - 2] = "1";
	}
	else {
		bigmap[mapY - 1][mapX - 2] = "0";
	}
	if (field[tile + COLS].W) {
		if (bigmap[mapY + 1][mapX - 2] == "0") bigmap[mapY + 1][mapX - 2] = "1";
		if (bigmap[mapY][mapX - 2] != "!") bigmap[mapY][mapX - 2] = "1";
	}
	else {
		bigmap[mapY + 1][mapX - 2] = "0";
	}
	if (!field[tile].W && !field[tile + COLS].W && field[tile - 1].S) {
		bigmap[mapY][mapX - 2] = "1";
		bigmap[mapY][mapX - 3] = "1";
	}
	// Corners
	if (bigmap[mapY - 2][mapX - 2] != "!" && ((field[tile].N && field[tile].W) || (bigmap[mapY - 2][mapX - 1] == "1" && bigmap[mapY - 2][mapX - 3] == "1") || (bigmap[mapY - 1][mapX - 2] == "1" && bigmap[mapY - 3][mapX - 2] == "1"))) {
		bigmap[mapY - 2][mapX - 2] = "1";
	}
	if (bigmap[mapY - 2][mapX + 2] != "!" && ((field[tile + 1].N && field[tile + 1].E) || (bigmap[mapY - 2][mapX + 1] == "1" && bigmap[mapY - 2][mapX + 3] == "1") || (bigmap[mapY - 1][mapX + 2] == "1" && bigmap[mapY - 3][mapX + 2] == "1"))) {
		bigmap[mapY - 2][mapX + 2] = "1";
	}
	if (bigmap[mapY + 2][mapX - 2] != "!" && ((field[tile + COLS].S && field[tile + COLS].W) || (bigmap[mapY + 2][mapX - 1] == "1" && bigmap[mapY + 2][mapX - 3] == "1") || (bigmap[mapY + 1][mapX - 2] == "1" && bigmap[mapY + 3][mapX - 2] == "1"))) {
		bigmap[mapY + 2][mapX - 2] = "1";
	}
	if (bigmap[mapY + 2][mapX + 2] != "!" && ((field[tile + COLS + 1].S && field[tile + COLS + 1].E) || (bigmap[mapY + 2][mapX + 1] == "1" && bigmap[mapY + 2][mapX + 3] == "1") || (bigmap[mapY + 1][mapX + 2] == "1" && bigmap[mapY + 3][mapX + 2] == "1"))) {
		bigmap[mapY + 2][mapX + 2] = "1";
	}
	
	char color = field[tile].color + '0';
	//printf("%c\n", color);
	if (color != '0') {
		bigmap[mapY - 1][mapX - 1] = { color };
		bigmap[mapY - 1][mapX + 1] = { color };
		bigmap[mapY + 1][mapX - 1] = { color };
		bigmap[mapY + 1][mapX + 1] = { color };
	}
}

void readMapTile(int tile) {
	updateMapCoords(tile);
	// North
	if (bigmap[mapY - 2][mapX - 1] == "1") {
		field[tile].N = 1;
	}
	if (bigmap[mapY - 2][mapX + 1] == "1") {
		field[tile + 1].N = 1;
	}
	// East
	if (bigmap[mapY - 1][mapX + 2] == "1") {
		field[tile + 1].E = 1;
	}
	if (bigmap[mapY + 1][mapX + 2] == "1") {
		field[tile + COLS + 1].E = 1;
	}
	// South
	if (bigmap[mapY + 2][mapX - 1] == "1") {
		field[tile + COLS].S = 1;
	}
	if (bigmap[mapY + 2][mapX + 1] == "1") {
		field[tile + COLS + 1].S = 1;
	}
	// West
	if (bigmap[mapY - 1][mapX - 2] == "1") {
		field[tile].W = 1;
	}
	if (bigmap[mapY + 1][mapX - 2] == "1") {
		field[tile + COLS].W = 1;
	}
}

// Map victim
void mapVictim(int tile, int direction, char code) {
	string *p = 0;
	updateMapCoords(tile);
	switch (direction) {
		case North:
			p = &bigmap[mapY - 2][mapX + 1];
			break;
		case East:
			p = &bigmap[mapY + 1][mapX + 2];
			break;
		case South:
			p = &bigmap[mapY + 2][mapX + 1];
			break;
		case West:
			p = &bigmap[mapY + 1][mapX - 2];
			break;
	}
	if (*p == "1") *p = { code };
	else if (*p != "0") *p += { code };
}

void mapAngledVictim(int tile, int direction, char code) {
	string *p = 0;
	enum Directions { N0, N1, E0, E1, S0, S1, W0, W1 };
	updateMapCoords(tile);
	switch (direction) {
		case N0:
			p = &bigmap[mapY - 2][mapX - 1];
			break;
		case N1:
			p = &bigmap[mapY - 2][mapX + 1];
			break;
		case E0:
			p = &bigmap[mapY - 1][mapX + 2];
			break;
		case E1:
			p = &bigmap[mapY + 1][mapX + 2];
			break;
		case S0:
			p = &bigmap[mapY + 2][mapX + 1];
			break;
		case S1:
			p = &bigmap[mapY + 2][mapX - 1];
			break;
		case W0:
			p = &bigmap[mapY + 1][mapX - 2];
			break;
		case W1:
			p = &bigmap[mapY - 1][mapX - 2];
			break;
	}

	if (*p == "1") *p = { code };
	else if (*p != "0") *p += { code };
}

void mapCurvedWall(int tile, int subTile, int curveDirection) {
	switch (subTile) {
		case 0:
			if (curveDirection == tl) {
				if (bigmap[mapY - 3][mapX - 2] == "1" || bigmap[mapY - 2][mapX - 3] == "1")
					break;
				bigmap[mapY - 2][mapX - 2] = "!";
			}
			break;
		case 1:
			if (curveDirection == tr) {
				if (bigmap[mapY - 3][mapX + 2] == "1" || bigmap[mapY - 2][mapX + 3] == "1")
					break;
				bigmap[mapY - 2][mapX + 2] = "!";
			}
			break;
		case 2:
			if (curveDirection == br) {
				if (bigmap[mapY + 3][mapX + 2] == "1" || bigmap[mapY + 2][mapX + 3] == "1")
					break;
				bigmap[mapY + 2][mapX + 2] = "!";
			}
			break;
		case 3:
			if (curveDirection == bl) {
				if (bigmap[mapY + 3][mapX - 2] == "1" || bigmap[mapY + 2][mapX - 3] == "1")
					break;
				bigmap[mapY + 2][mapX - 2] = "!";
			}
			break;
	}
}

bool obstacle(int tile) {
	float dist = 5;
	bool left = 0, right = 0;
	bool wall[8] = { 0 };
	
	for (int i = 442; i < 512; i++) {
		//printf("%d %f\n", i, bot.getLidarPoint(3, i));
		if (bot.getLidarPoint(3, i) < dist) {
			printf("left\n");
			left = 1;
			if (bot.getLidarPoint(3, 12) < 7) {
				printf("right\n");
				right = 1;
			}
			break;
		}
	}
	for (int i = 0; i < 70; i++) {
		if (bot.getLidarPoint(3, i) < dist) {
			printf("right\n");
			right = 1;
			if (bot.getLidarPoint(3, 500) < 7) {
				printf("left\n");
				left = 1;
			}
			break;
		}
	}
	
		switch (bot.getDirection()) {
			case North:
				if (left && right) {
					setWalls(tile - COLS * 2, 1, 1, 1, 1);
					setWalls(tile - COLS * 2 + 1, 1, 1, 1, 1);
					setWalls(tile - COLS, 1, 1, 1, 1);
					setWalls(tile - COLS + 1, 1, 1, 1, 1);
				}
				else if (left && !right) {
					setWalls(tile - COLS * 2 - 1, 1, 1, 1, 1);
					setWalls(tile - COLS * 2, 1, 1, 1, 1);
					setWalls(tile - COLS - 1, 1, 1, 1, 1);
					setWalls(tile - COLS, 1, 1, 1, 1);
				}
				else if (right && !left) {
					setWalls(tile - COLS * 2 + 1, 1, 1, 1, 1);
					setWalls(tile - COLS * 2 + 2, 1, 1, 1, 1);
					setWalls(tile - COLS + 1, 1, 1, 1, 1);
					setWalls(tile - COLS + 2, 1, 1, 1, 1);
				}
				break;
			case East:
				if (left && right) {
					setWalls(tile + 2, 1, 1, 1, 1);
					setWalls(tile + 3, 1, 1, 1, 1);
					setWalls(tile + COLS + 2, 1, 1, 1, 1);
					setWalls(tile + COLS + 3, 1, 1, 1, 1);
				}
				else if (left && !right) {
					setWalls(tile - COLS + 2, 1, 1, 1, 1);
					setWalls(tile - COLS + 3, 1, 1, 1, 1);
					setWalls(tile + 2, 1, 1, 1, 1);
					setWalls(tile + 3, 1, 1, 1, 1);
				}
				else if (right && !left) {
					setWalls(tile + COLS + 2, 1, 1, 1, 1);
					setWalls(tile + COLS + 3, 1, 1, 1, 1);
					setWalls(tile + COLS * 2 + 2, 1, 1, 1, 1);
					setWalls(tile + COLS * 2 + 3, 1, 1, 1, 1);
				}
				break;
			case South: // fix this
				if (left && right) {
					setWalls(tile + COLS * 2, 1, 1, 1, 1);
					setWalls(tile + COLS * 2 + 1, 1, 1, 1, 1);
					setWalls(tile + COLS, 1, 1, 1, 1);
					setWalls(tile + COLS + 1, 1, 1, 1, 1);
				}
				else if (left && !right) {
					setWalls(tile + COLS * 2 + 1, 1, 1, 1, 1);
					setWalls(tile + COLS * 2 + 2, 1, 1, 1, 1);
					setWalls(tile + COLS + 1, 1, 1, 1, 1);
					setWalls(tile + COLS + 2, 1, 1, 1, 1);
				}
				else if (right && !left) {
					setWalls(tile + COLS * 2 - 1, 1, 1, 1, 1);
					setWalls(tile + COLS * 2, 1, 1, 1, 1);
					setWalls(tile + COLS - 1, 1, 1, 1, 1);
					setWalls(tile + COLS, 1, 1, 1, 1);
				}
				break;
			case West:
				if (left && right) {
					setWalls(tile - 2, 1, 1, 1, 1);
					setWalls(tile - 1, 1, 1, 1, 1);
					setWalls(tile + COLS - 2, 1, 1, 1, 1);
					setWalls(tile + COLS - 1, 1, 1, 1, 1);
				}
				else if (left && !right) {
					setWalls(tile + COLS - 2, 1, 1, 1, 1);
					setWalls(tile + COLS - 1, 1, 1, 1, 1);
					setWalls(tile + COLS * 2 - 2, 1, 1, 1, 1);
					setWalls(tile + COLS * 2 - 1, 1, 1, 1, 1);
				}
				else if (right && !left) {
					setWalls(tile - COLS - 2, 1, 1, 1, 1);
					setWalls(tile - COLS - 1, 1, 1, 1, 1);
					setWalls(tile - 2, 1, 1, 1, 1);
					setWalls(tile - 1, 1, 1, 1, 1);
				}
				break;
		}

	if (left || right) return true;
	return false;
}

// Get tile data(set bits of walls)
void getTile(int tile) {
	if (bot.getDirection() < 0) return;
	const float concaveThreshold = 6.8;
	const float convexThreshold = 8.4;
	const int outer[] = { tile - COLS, tile - COLS + 1, tile + 2, tile + 2 + COLS, tile + COLS + COLS, tile + COLS + COLS + 1, tile - 1, tile - 1 + COLS }; // neighbors of sub-tiles
	const int directions[4] = { tile, tile + 1, tile + COLS + 1, tile + COLS }; // sub-tiles
	const int n = 30; // lidar range
	const int check[] = { 511 - n, 0 + n, 127 - n, 127 + n, 255 - n, 255 + n, 383 - n, 383 + n }; // lidar values to check
	const int distFromWall = 9;
	int dir = bot.getDirection();
	int bit = 1;
	int big = 0;
	int small = 9999;
	double prevValue = 0;
	double diff = 0;

	bool wall[8] = { 0 };

	if (field[tile].visited || dir < 0) return;

	// clear structure
	for (int i = 0; i < 4; i++) {
		field[directions[i]].N = field[directions[i]].E = field[directions[i]].S = field[directions[i]].W = 0;
		for (int j = 0; j < 4; j++) {
			//field[directions[i]].corner = -1;
		}
	}
	//readMapTile(tile);
	// Scan for walls with Lidar
	for (int i = 0; i < sizeof(check) / sizeof(check[0]); i++) {
		if (bot.getLidarPoint(3, check[i]) < distFromWall) {
			// robot facing west
			if (i + bot.getDirection() * 2 > 7) {
				wall[i + bot.getDirection() * 2 - 8] = true;
			}
			else {
				wall[i + bot.getDirection() * 2] = true;
			}
			//printf("wall dir %d bot dir %d real dir %d\n", i, bot.getDirection(), (i + bot.getDirection() * 2 > 7) ? (i + bot.getDirection() * 2) - 8 : i + bot.getDirection() * 2);
			//field[directions[dir]].bits += bit; // set wall bit
		}
		// odd:        next wall  next tile
		(i % 2 != 0) ? bit *= 2 : (dir == West) ? dir = North : dir++;
	}
	
	// Scan for walls with Lidar
	//for (int i = 0; i < sizeof(check) / sizeof(check[0]); i++) {
	//	//std::vector<float> v;
	//	//float average = 0;
	//	//big = 0;
	//	//small = 9999;
	//	int start = check[i];
	//	//printf("STUFF i %d start %d check %I64u\n", i, start, sizeof(check) / sizeof(check[0]));
	//	// look at range
	//	for (int j = start - n / 2; j < start + n / 2; j++) {
	//		if (bot.getLidarPoint(3, j) == INFINITY) continue; // sometimes it gets inf for some reason... :(


	//		/*if (prevValue != 0) {
	//			diff = bot.getLidarPoint(3, j) - prevValue;
	//			printf("DIFF %d %f\n", i, diff);
	//		}

	//		prevValue = bot.getLidarPoint(3, j);*/


	//		// get furthest and closest lidar values in range
	//		/*if (bot.getLidarPoint(3, j) > big) {
	//			big = bot.getLidarPoint(3, j);
	//		}
	//		if (bot.getLidarPoint(3, j) < small) {
	//			small = bot.getLidarPoint(3, j);
	//		}*/
	//		//v.push_back(bot.getLidarPoint(3, j));
	//		//printf("%d %f\n", j, bot.getLidarPoint(3, j));

	//	//average = std::accumulate(v.begin(), v.end(), 0.0) / v.size();
	//	//v.clear();
	//	//printf("i = %d, diff = %d big %d small %d\n", i, big - small, big, small);
	//	//// theres an obstacle (skewed)
	//	//if (big - small > 18 && small < 11) {
	//	//	// figure out which tile the obstacle is in
	//	//	int dir = i / 2 + bot.getDirection();
	//	//	if (dir >= 4) dir -= 4;

	//	//	field[directions[dir]].obstacle = 1;

	//	//	printf("OBSTACLE %d %d %d\n", i, bot.getDirection(), dir);
	//	//	bot.delay(1000);
	//	//}
	//		if (bot.getLidarPoint(3, j) < distFromWall) {
	//			// robot facing west
	//			if (i + bot.getDirection() * 2 > 7) {
	//				wall[i + bot.getDirection() * 2 - 8] = true;
	//			}
	//			else {
	//				wall[i + bot.getDirection() * 2] = true;
	//			}
	//			//printf("wall dir %d bot dir %d real dir %d\n", i, bot.getDirection(), (i + bot.getDirection() * 2 > 7) ? (i + bot.getDirection() * 2) - 8 : i + bot.getDirection() * 2);
	//			//field[directions[dir]].bits += bit; // set wall bit
	//		}
	//		
	//	}
	//	// odd:        next wall  next tile
	//		(i % 2 != 0) ? bit *= 2 : (dir == West) ? dir = North : dir++;
	//}
	
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
			if (bot.getLidarPoint(3, i * 128 - 30) < 7 || bot.getLidarPoint(3, i * 128 + 30) < 7) break;
			if (bot.getLidarPoint(3, j) < 7) {
				int wall = i + dir; // what side the wall is on
				if (wall >= 4) wall -= 4; // fix side

				// set bits
				field[outer[wall * 2]].bits |= (wall % 2 == 0) ? 2 : 4;
				field[outer[wall * 2 + 1]].bits |= (wall % 2 == 0) ? 8 : 1;

				//printf("mid wall thingy  dir %d\n", i);
				//printf("WALL %d\n", wall);
				//printf("tile %d %d\n", outer[wall * 2], (wall % 2 == 0) ? 2 : 4);
				//printf("tile %d %d\n", outer[wall * 2 + 1], (wall % 2 == 0) ? 8 : 1);
				break;
			}
		}
	}
	//}

	editMapTile(tile);

	if (bot.curRoom == 3) {
		int realTile = 0;
		int realDir = 0;
		float lidarValue = 0;

		//printf("CURVED CHECK\n");
		//printf("%f %f %f %f %f\n", bot.getLidarPoint(3, 0), bot.getLidarPoint(3, 18), bot.getLidarPoint(3, 32), bot.getLidarPoint(3, 65), bot.getLidarPoint(3, 96));

		std::vector<float> v;
		double average[4] = { 0 };

		for (int start = 0; start < 385; start += 128) {
			for (int i = 0; i < 4; i++) {
				for (int j = i * 32 + start; j < i * 32 + 32 + start; j++) {
					if (bot.getLidarPoint(3, j) == INFINITY) continue;
					v.push_back(bot.getLidarPoint(3, j));
				}
				average[i] = std::accumulate(v.begin(), v.end(), 0.0) / v.size();
				//printf("%d avg%d: %f\n", start, i, average[i]);
				v.clear();
			}

			// concave
			if (average[1] < 7 && fabs(average[1] - average[0]) < 0.6 && fabs(average[3] - average[2]) < 0.6) {
				printf("%d concave\n", start / 128);
				
				// Find what sub-tile the curve is in
				int curvetile = start / 128 + 1;
				if (curvetile > 3) curvetile -= 4;

				// fix direction
				curvetile += bot.getDirection();
				if (curvetile >= 4) curvetile -= 4;

				//printf("thingy %d\n", curvetile);

				mapCurvedWall(tile, curvetile, curvetile);
			}

			// front convex
			/*if (average[3] < 8 && average[2] < average[3]) {
				printf("CONVEX FRONT\n");
			}*/

			// side convex
		}


		/*printf("%f %f %f %f\n", bot.getLidar(3, 446), bot.getLidar(3, 65), bot.getLidar(3, 190), bot.getLidar(3, 321));
		printf("%f %f %f\n", bot.getLidar(3, 0), bot.getLidar(3, 255), bot.getLidar(3, 383));*/
		//if ((lidarValue = bot.getLidarPoint(3, 446)) < convexThreshold) {
		//	// convex
		//	if (bot.getLidarPoint(3, 383) > lidarValue && bot.getLidarPoint(3, 383) < 12) {
		//		printf("CONVEX 446west\n");
		//	}
		//	else if (lidarValue > bot.getLidarPoint(3, 480) && lidarValue - bot.getLidarPoint(3, 480) < 1.5) {
		//		printf("CONVEX 446north\n");
		//	}
		//	// concave
		//	else if (lidarValue < concaveThreshold) {
		//		realTile = directions[bot.getDirection()];
		//		realDir = tl + bot.getDirection();
		//		field[realTile].corner = realDir;
		//		printf("%d %d\n", realTile, realDir);
		//	}
		//}
		//if ((lidarValue = bot.getLidarPoint(3, 65)) < convexThreshold) {
		//	if (bot.getLidarPoint(3, 127) > lidarValue && bot.getLidarPoint(3, 127) < 12) {
		//		printf("CONVEX 65east\n");
		//	}
		//	else if (lidarValue > bot.getLidarPoint(3, 32) && lidarValue - bot.getLidarPoint(3, 32) < 1.5) {
		//		printf("CONVEX 65north\n");
		//	}
		//	else if (lidarValue < concaveThreshold) {
		//		realTile = directions[(bot.getDirection() + 1 > 3) ? 0 : bot.getDirection() + 1];
		//		realDir = (tr + bot.getDirection() > 3) ? 0 : tr + bot.getDirection();
		//		field[realTile].corner = realDir;
		//		printf("%d %d\n", realTile, realDir);
		//	}
		//}
		//if ((lidarValue = bot.getLidarPoint(3, 190)) < convexThreshold) {
		//	if (bot.getLidarPoint(3, 127) > lidarValue && bot.getLidarPoint(3, 127) < 12) {
		//		printf("CONVEX 190east\n");
		//	}
		//	else if (bot.getLidarPoint(3, 255) > lidarValue && bot.getLidarPoint(3, 255) < 12) {
		//		printf("CONVEX 190south\n");
		//	}
		//	else if (lidarValue < concaveThreshold) {
		//		realTile = directions[(bot.getDirection() + 2 > 3) ? bot.getDirection() - 2 : bot.getDirection() + 2];
		//		realDir = (br + bot.getDirection() > 3) ? 1 : br + bot.getDirection();
		//		field[realTile].corner = realDir;
		//		printf("%d %d\n", realTile, realDir);
		//	}
		//}
		//if ((lidarValue = bot.getLidarPoint(3, 321)) < convexThreshold) {
		//	if (bot.getLidarPoint(3, 383) > lidarValue && bot.getLidarPoint(3, 383) < 12) {
		//		printf("CONVEX 321west\n");
		//	}
		//	else if (bot.getLidarPoint(3, 255) > lidarValue && bot.getLidarPoint(3, 255) < 12) {
		//		printf("CONVEX 321south\n");
		//	}
		//	else if (lidarValue < concaveThreshold) {
		//		realTile = directions[(bot.getDirection() + 3 > 3) ? bot.getDirection() - 1 : bot.getDirection() + 3];
		//		realDir = (bl + bot.getDirection() > 3) ? 2 : bl + bot.getDirection();
		//		field[realTile].corner = realDir;
		//		printf("%d %d\n", realTile, realDir);
		//	}
		//}
		//bot.delay(1000);
	}

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

	if ((tile == bot.redTile || tile == bot.greenTile)) {
		printf("on room 4 connection tile\n");
		if (bot.LOProom4) {
			switch (bot.getDirection()) {
				case North:
					setWalls(tile, 0, 0, 0, 1);
					setWalls(tile + 1, 0, 1, 0, 0);
					setWalls(tile + COLS, 0, 0, 1, 1);
					setWalls(tile + COLS + 1, 0, 1, 1, 0);
					break;
				case East:
					setWalls(tile, 1, 0, 0, 1);
					setWalls(tile + 1, 1, 0, 0, 0);
					setWalls(tile + COLS, 0, 0, 1, 1);
					setWalls(tile + COLS + 1, 0, 0, 1, 0);
					break;
				case South:
					setWalls(tile, 1, 0, 0, 1);
					setWalls(tile + 1, 1, 1, 0, 0);
					setWalls(tile + COLS, 0, 1, 0, 0);
					setWalls(tile + 1, 0, 0, 0, 1);
					break;
				case West:setWalls(tile, 1, 0, 0, 0);
					setWalls(tile + 1, 1, 1, 0, 0);
					setWalls(tile + COLS, 0, 0, 1, 0);
					setWalls(tile + COLS + 1, 0, 1, 1, 0);
					break;
			}
		}
		else {
			switch (bot.getDirection()) {
				case North:
					setWalls(tile, 1, 0, 0, 1);
					setWalls(tile + 1, 1, 1, 0, 0);
					setWalls(tile + COLS, 0, 1, 0, 0);
					setWalls(tile + 1, 0, 0, 0, 1);
					break;
				case East:
					setWalls(tile, 1, 0, 0, 0);
					setWalls(tile + 1, 1, 1, 0, 0);
					setWalls(tile + COLS, 0, 0, 1, 0);
					setWalls(tile + COLS + 1, 0, 1, 1, 0);
					break;
				case South:
					setWalls(tile, 0, 0, 0, 1);
					setWalls(tile + 1, 0, 1, 0, 0);
					setWalls(tile + COLS, 0, 0, 1, 1);
					setWalls(tile + COLS + 1, 0, 1, 1, 0);
					break;
				case West:
					setWalls(tile, 1, 0, 0, 1);
					setWalls(tile + 1, 1, 0, 0, 0);
					setWalls(tile + COLS, 0, 0, 1, 1);
					setWalls(tile + COLS + 1, 0, 0, 1, 0);
					break;
			}
		}
	}


	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {
			cout << bigmap[mapY - 2 + i][mapX - 2 + j] << " ";
			//printf("%s ", bigmap[mapY - 2 + i][mapX - 2 + j]);
		}
		printf("\n");
	}

	if (!field[tile].N && !field[tile + 1].N && !field[tile - COLS].victimChecked && !field[tile - COLS + 1].victimChecked && field[tile - COLS].color == Hole && field[tile - COLS + 1].color == Hole) {
		bot.turn(North);
		getBlackHole(tile);
	}
	if (!field[tile + 1].E && !field[tile + COLS + 1].E && !field[tile + 2].victimChecked && !field[tile + COLS + 2].victimChecked && field[tile + 2].color == Hole && field[tile + COLS + 2].color == Hole) {
		bot.turn(East);
		getBlackHole(tile);
	}
	if (!field[tile + COLS].S && !field[tile + COLS + 1].S && !field[tile + COLS * 2].victimChecked && !field[tile + COLS * 2 + 1].victimChecked && field[tile + COLS * 2].color == Hole && field[tile + COLS * 2 + 1].color == Hole) {
		bot.turn(South);
		getBlackHole(tile);
	}
	if (!field[tile].W && !field[tile + COLS].W && !field[tile - 1].victimChecked && !field[tile + COLS - 1].victimChecked && field[tile - 1].color == Hole && field[tile + COLS - 1].color == Hole) {
		bot.turn(West);
		getBlackHole(tile);
	}
}

void getBlackHole(int tile) {
	printf("BLACK HOLE WALL SCAN\n");
	switch (bot.getDirection()) {
		case North:
			field[tile - COLS * 2].victimChecked = field[tile - COLS * 2 + 1].victimChecked = field[tile - COLS].victimChecked = field[tile - COLS + 1].victimChecked = 1;
			tile -= COLS * 2;
			break;
		case East:
			field[tile + 2].victimChecked = field[tile + 3].victimChecked = field[tile + COLS + 2].victimChecked = field[tile + COLS + 3].victimChecked = 1;
			tile += 2;
			break;
		case South:
			field[tile + COLS * 2].victimChecked = field[tile + COLS * 2 + 1].victimChecked = field[tile + COLS * 3].victimChecked = field[tile + COLS * 3 + 1].victimChecked = 1;
			tile += COLS * 2;
			break;
		case West:
			field[tile - 2].victimChecked = field[tile - 1].victimChecked = field[tile + COLS - 2].victimChecked = field[tile + COLS - 1].victimChecked = 1;
			tile -= 2;
			break;
	}
	bool wall[8] = { 0 };
	const int wallThreshold = 16;
	const int check[] = { 507, 5, 40, 70, 0, 0, 441, 471 };

	for (int i = 0; i < 8; i++) {
		if (check[i] == 0) continue;
		if (bot.getLidarPoint(3, check[i]) < wallThreshold) {
			printf("%d %f\n", i, bot.getLidarPoint(3, check[i]));
			if (i + bot.getDirection() * 2 > 7) {
				wall[i + bot.getDirection() * 2 - 8] = true;
			}
			else {
				wall[i + bot.getDirection() * 2] = true;
			}
		}
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
	editMapTile(tile);
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

// Mapping bonus
void mapBonus() {
	unsigned int width, height, startX = 0, startY = 0, endX, endY, rows, cols;

	// Calculate Height and Width of map
	while (isEmptyCol(startX)) startX++;
	endX = startX;
	while (!isEmptyCol(endX)) endX++;

	while (isEmptyRow(startY)) startY++;
	endY = startY;
	while (!isEmptyRow(endY)) endY++;

	printf("endY %d startY %d endX %d startX %d\n", endY, startY, endX, startX);

	rows = (endY - startY - 1) / 2 + 1;
	cols = (endX - startX - 1) / 2 + 1;
	width = cols * 4 + 1;
	height = rows * 4 + 1;
	printf("Height = %d\n", height);
	printf("Width = %d\n", width);
	printf("Rows = %d\nCols = %d\n", rows, cols);
	
	vector<vector<string>> realmap(height, vector<string>(width));
	while (bot.update()) {
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				if (bigmap[startY * 2 + i][startX * 2 + j] == "!") bigmap[startY * 2 + i][startX * 2 + j] = "0";
				realmap[i][j] = bigmap[startY * 2 + i][startX * 2 + j];
				cout << realmap[i][j] << " ";
			}
			cout << endl;
		}
		break;
	}

	string flattened = "";
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			flattened += realmap[i][j] + ","; // Flatten the array with comma separators
		}
	}

	flattened.pop_back(); // Remove the last unnecessary comma

	char *message = (char *)malloc(8 + flattened.size());
	// The first 2 integers in the message array are width, height
	memcpy(message, &height, sizeof(height));
	memcpy(&message[4], &width, sizeof(width));
	memcpy(&message[8], flattened.c_str(), flattened.size()); // Copy in the flattened map afterwards
	bot.emitter->send(message, 8 + flattened.size()); // Send map data
	char msg = 'M'; // Send map evaluate request
	bot.emitter->send(&msg, sizeof(msg));

	//const int width = 13, height = 13;
	//string map[height][width] = {
	//	{"1", "1", "1", "1", "1", "1", "1", "H", "1", "1", "1", "S", "1"},
	//	{"1", "5", "0", "5", "0", "0", "0", "0", "0", "0", "0", "0", "1"},
	//	{"1", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "1"},
	//	{"1", "5", "0", "5", "0", "0", "0", "0", "0", "0", "0", "0", "1"},
	//	{"1", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "1"},
	//	{"H", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "1"},
	//	{"1", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "1"},
	//	{"1", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "S"},
	//	{"1", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "1"},
	//	{"1", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "1"},
	//	{"1", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "1"},
	//	{"1", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "1"},
	//	{"1", "1", "1", "U", "1", "H", "1", "1", "1", "S", "1", "1", "1"}
	//};
	//
	//string flattened = "";
	//for (int i = 0; i < height; i++) {
	//	for (int j = 0; j < width; j++) {
	//		flattened += map[i][j] + ","; // Flatten the array with comma separators
	//	}
	//}

	//flattened.pop_back(); // Remove the last unnecessary comma

	//char *message = (char *)malloc(8 + flattened.size());
	//// The first 2 integers in the message array are width, height
	//memcpy(message, &height, sizeof(height));
	//memcpy(&message[4], &width, sizeof(width));
	//memcpy(&message[8], flattened.c_str(), flattened.size()); // Copy in the flattened map afterwards
	//bot.emitter->send(message, 8 + flattened.size()); // Send map data
	//char msg = 'M'; // Send map evaluate request
	//bot.emitter->send(&msg, sizeof(msg));
}

bool compareX(double x1, double x2) {
	if (fabs(x1) - fabs(x2) > 0.05 || fabs(x1) - fabs(x2) < -0.05) {
		return 0;
	}
	return 1;
}

bool compareY(double y1, double y2) {
	if (fabs(y1) - fabs(y2) > 0.05 || fabs(y1) - fabs(y2) < -0.05) {
		return 0;
	}
	return 1;
}

bool compareCoords(double x1, double y1, double x2, double y2) {
	printf("X: %f Y: %f\n", fabs(x1) - fabs(x2), fabs(y1) - fabs(y2));
	if (compareX(x1, x2) && compareY(y1, y2)) return 1;
	return 0;
}

int coordToTile(double x, double y) {
	int tile = bot.startTile;
	if (bot.startPos.x < x) {
		while (!compareX(x, bot.startPos.x)) {
			x -= 0.06;
			tile++;
		}
	}
	else {
		while (!compareX(x, bot.startPos.x)) {
			x += 0.06;
			tile--;
		}
	}
	if (bot.startPos.y < y) {
		while (!compareX(y, bot.startPos.y)) {
			y -= 0.06;
			tile += COLS;
		}
	}
	else {
		while (!compareX(y, bot.startPos.y)) {
			y += 0.06;
			tile -= COLS;
		}
	}
	return tile;
}
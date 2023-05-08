#include "tile.h"

using namespace std;

// Shape of curve. Shape looks like a quarter circle
enum cornerDirection { tl, tr, br, bl };

char bigmap[3 * ROWS + ROWS + 1][3 * COLS + COLS + 1] = { 0 };
int mapX = (bot.curTile % COLS) * 2 + 2, mapY = (bot.curTile / ROWS) * 2 + 2;
Tile field[fieldSize];

void updateMapCoords() {
	mapX = (bot.curTile % COLS) * 2 + 2;
	mapY = (bot.curTile / ROWS) * 2 + 2;
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

void editMapTile(int tile) {
	updateMapCoords();
	// North
	if (field[tile].N) {
		bigmap[mapY - 2][mapX - 1] = '1';
	}
	if (field[tile + 1].N) {
		bigmap[mapY - 2][mapX + 1] = '1';
	}
	if (field[tile].N && field[tile + 1].N) {
		bigmap[mapY - 2][mapX] = '1';
	}
	// East
	if (field[tile + 1].E) {
		bigmap[mapY - 1][mapX + 2] = '1';
	}
	if (field[tile + COLS + 1].E) {
		bigmap[mapY + 1][mapX + 2] = '1';
	}
	if (field[tile + 1].E && field[tile + COLS + 1].E) {
		bigmap[mapY][mapX + 2] = '1';
	}
	// South
	if (field[tile + COLS].S) {
		bigmap[mapY + 2][mapX - 1] = '1';
	}
	if (field[tile + COLS + 1].S) {
		bigmap[mapY + 2][mapX + 1] = '1';
	}
	if (field[tile + COLS].S && field[tile + COLS + 1].S) {
		bigmap[mapY + 2][mapX] = '1';
	}
	// West
	if (field[tile].W) {
		bigmap[mapY - 1][mapX - 2] = '1';
	}
	if (field[tile + COLS].W) {
		bigmap[mapY + 1][mapX - 2] = '1';
	}
	if (field[tile].W && field[tile + COLS].W) {
		bigmap[mapY][mapX - 2] = '1';
	}
	// Corners
	if (field[tile].N && field[tile].W) {
		bigmap[mapY - 2][mapX - 2] = '1';			
	}
	if (field[tile + 1].N && field[tile + 1].E) {
		bigmap[mapY - 2][mapX + 2] = '1';
	}
	if (field[tile + COLS].S && field[tile + COLS].W) {
		bigmap[mapY + 2][mapX - 2] = '1';
	}
	if (field[tile + COLS + 1].S && field[tile + COLS + 1].E) {
		bigmap[mapY + 2][mapX + 2] = '1';
	}

	// Start tile
	if (tile == bot.startTile) {
		bigmap[mapY - 1][mapX - 1] = '5';
		bigmap[mapY - 1][mapX + 1] = '5';
		bigmap[mapY + 1][mapX - 1] = '5';
		bigmap[mapY + 1][mapX + 1] = '5';
	}
}

// Get tile data(set bits of walls)
void getTile(int tile) {
	printf("%d %d\n", bot.curTile % COLS, bot.curTile / ROWS);
	if (bot.getDirection() < 0) return;
	const float concaveThreshold = 6.8;
	const float convexThreshold = 8.4;
	const int outer[] = { tile - COLS, tile - COLS + 1, tile + 2, tile + 2 + COLS, tile + COLS + COLS, tile + COLS + COLS + 1, tile - 1, tile - 1 + COLS }; // neighbors of sub-tiles
	const int directions[4] = { tile, tile + 1, tile + COLS + 1, tile + COLS }; // sub-tiles
	const int n = 30; // lidar range
	const int check[] = { 511 - n, 0 + n, 127 - n, 127 + n, 255 - n, 255 + n, 383 - n, 383 + n }; // lidar values to check
	const int distFromWall = 8;
	int dir = bot.getDirection();
	int bit = 1;

	bool wall[8] = { 0 };

	//if (field[tile].visited || dir < 0) return;
	// clear structure
	for (int i = 0; i < 4; i++) {
		field[directions[i]].N = field[directions[i]].E = field[directions[i]].S = field[directions[i]].W = 0;
		for (int j = 0; j < 4; j++) {
			field[directions[i]].corner = -1;
		}
	}

	// Scan for walls with Lidar
	for (int i = 0; i < sizeof(check) / sizeof(check[0]); i++) {
		if (bot.getLidar(3, check[i]) < distFromWall) {
			// robot facing west
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
				printf("tile %d %d\n", outer[wall * 2 + 1], (wall % 2 == 0) ? 8 : 1);
				break;
			}
		}
	}
	//}

	if (bot.curRoom == 3) {
		int realTile = 0;
		int realDir = 0;
		float lidarValue = 0;

		printf("CURVED CHECK\n");
		printf("%f %f %f %f %f\n", bot.getLidar(3, 0), bot.getLidar(3, 18), bot.getLidar(3, 32), bot.getLidar(3, 65), bot.getLidar(3, 96));

		std::vector<float> v;
		double average[4] = { 0 };
		for (int start = 0; start < 385; start += 128) {
			for (int i = 0; i < 4; i++) {
				for (int j = i * 32 + start; j < i * 32 + 32 + start; j++) {
					if (bot.getLidar(3, j) == INFINITY) continue;
					v.push_back(bot.getLidar(3, j));
				}
				average[i] = std::accumulate(v.begin(), v.end(), 0.0) / v.size();
				printf("avg%d: %f\n", i, average[i]);
				v.clear();
			}
		
			if (average[1] < 7 && fabs(average[1] - average[0]) < 0.6 && fabs(average[3] - average[2]) < 0.6) {
				printf("%d concave\n", start);
			}
			else if (average[2] < 8 && fabs(average[2] - average[3]) < 0.8) {
				printf("%d side bottom\n", start);
			}
			else if (average[3] < 9 && fabs(average[2] - average[3]) > 2.0) {
				printf("%d side top\n", start);
			}
			else if (average[0] < 9) {
				if (fabs(average[0] - average[1]) < 1.0) {
					printf("%d front left\n", start);
				}
				else if (average[1] / average[0] > 1.0) {
					printf("%d front right\n", start);
				}
			}
		}

		/*printf("%f %f %f %f\n", bot.getLidar(3, 446), bot.getLidar(3, 65), bot.getLidar(3, 190), bot.getLidar(3, 321));
		printf("%f %f %f\n", bot.getLidar(3, 0), bot.getLidar(3, 255), bot.getLidar(3, 383));*/
		//if ((lidarValue = bot.getLidar(3, 446)) < convexThreshold) {
		//	// convex
		//	if (bot.getLidar(3, 383) > lidarValue && bot.getLidar(3, 383) < 12) {
		//		printf("CONVEX 446west\n");
		//	}
		//	else if (lidarValue > bot.getLidar(3, 480) && lidarValue - bot.getLidar(3, 480) < 1.5) {
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
		//if ((lidarValue = bot.getLidar(3, 65)) < convexThreshold) {
		//	if (bot.getLidar(3, 127) > lidarValue && bot.getLidar(3, 127) < 12) {
		//		printf("CONVEX 65east\n");
		//	}
		//	else if (lidarValue > bot.getLidar(3, 32) && lidarValue - bot.getLidar(3, 32) < 1.5) {
		//		printf("CONVEX 65north\n");
		//	}
		//	else if (lidarValue < concaveThreshold) {
		//		realTile = directions[(bot.getDirection() + 1 > 3) ? 0 : bot.getDirection() + 1];
		//		realDir = (tr + bot.getDirection() > 3) ? 0 : tr + bot.getDirection();
		//		field[realTile].corner = realDir;
		//		printf("%d %d\n", realTile, realDir);
		//	}
		//}
		//if ((lidarValue = bot.getLidar(3, 190)) < convexThreshold) {
		//	if (bot.getLidar(3, 127) > lidarValue && bot.getLidar(3, 127) < 12) {
		//		printf("CONVEX 190east\n");
		//	}
		//	else if (bot.getLidar(3, 255) > lidarValue && bot.getLidar(3, 255) < 12) {
		//		printf("CONVEX 190south\n");
		//	}
		//	else if (lidarValue < concaveThreshold) {
		//		realTile = directions[(bot.getDirection() + 2 > 3) ? bot.getDirection() - 2 : bot.getDirection() + 2];
		//		realDir = (br + bot.getDirection() > 3) ? 1 : br + bot.getDirection();
		//		field[realTile].corner = realDir;
		//		printf("%d %d\n", realTile, realDir);
		//	}
		//}
		//if ((lidarValue = bot.getLidar(3, 321)) < convexThreshold) {
		//	if (bot.getLidar(3, 383) > lidarValue && bot.getLidar(3, 383) < 12) {
		//		printf("CONVEX 321west\n");
		//	}
		//	else if (bot.getLidar(3, 255) > lidarValue && bot.getLidar(3, 255) < 12) {
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

	editMapTile(tile);

	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {
			printf("%c ", bigmap[mapY - 2 + i][mapX - 2 + j]);
		}
		printf("\n");
	}
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

void mapBonus() {
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
	
	vector<vector<string>> realmap(height, vector<string>(width));
	cout << realmap[0][0] << endl;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			realmap[i][j] = { bigmap[startY * 2 + i][startX * 2 + j] };
			cout << realmap[i][j] << " ";
		}
		printf("\n");
	}

	string flattened = "";
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			flattened += realmap[i][j] + ","; // Flatten the array with comma separators
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
#include "bfs.h"

int parent[fieldSize];
int worker[fieldSize];

// Check if valid neighbor
int findNeighbor(unsigned int tile, int dir) {
	if (bot.curRoom == 1) {
		unsigned int neighbors[] = { tile - COLS * 2, tile + 2, tile + COLS * 2, tile - 2 };
		switch (dir) {
			case North:
				if ((field[tile].bits & (1 << North)) == 0 && // TL N
					(field[tile + 1].bits & (1 << North)) == 0) // TR N
					return neighbors[dir];
				break;
			case East:
				if ((field[tile + 1].bits & (1 << East)) == 0 && // TR E
					(field[tile + COLS + 1].bits & (1 << East)) == 0) // BR E
					return neighbors[dir];
				break;
			case South:
				if ((field[tile + COLS].bits & (1 << South)) == 0 && // BL S
					(field[tile + COLS + 1].bits & (1 << South)) == 0) // BR S
					return neighbors[dir];
				break;
			case West:
				if ((field[tile].bits & (1 << West)) == 0 && // TL W
					(field[tile + COLS].bits & (1 << West)) == 0) // BL W 
					return neighbors[dir];
				break;
		}
	}
	else if (bot.curRoom == 2) {
		// tiles to the north, east, south, west
		unsigned int neighbors[] = { tile - COLS, tile + 1, tile + COLS, tile - 1 };
		switch (dir) {
			case North:
				if ((field[tile].bits & (1 << North)) == 0 &&
					(field[neighbors[East]].bits & (1 << North)) == 0 &&
					(field[neighbors[North]].bits & (1 << East)) == 0) {
					return neighbors[dir];
				}
				break;
			case East:
				if ((field[neighbors[East]].bits & (1 << East)) == 0 &&
					(field[neighbors[South] + 1].bits & (1 << East)) == 0 &&
					(field[neighbors[East] + 1].bits & (1 << South)) == 0) {
					return neighbors[dir];
				}
				break;
			case South:
				if ((field[neighbors[South]].bits & (1 << South)) == 0 &&
					(field[neighbors[South] + 1].bits & (1 << South)) == 0 &&
					(field[neighbors[South] + COLS].bits & (1 << East)) == 0) {
					return neighbors[dir];
				}
				break;
			case West:
				if ((field[tile].bits & (1 << West)) == 0 &&
					(field[neighbors[South]].bits & (1 << West)) == 0 &&
					(field[neighbors[West]].bits & (1 << South)) == 0) {
					return neighbors[dir];
				}
				break;
		}
	}

	return -1; // Tile is not a neighbor
}

// Breadth First Search
int bfs(int tile) {
	int *cur = &worker[0];
	int *next = cur + 1;
	int neighbor = -1;
	int target = -1;

	// reset parent and worker arrays
	memset(worker, -1, fieldSize * sizeof(int));
	memset(parent, -1, fieldSize * sizeof(int));

	*cur = tile; // add first tile to worker array
	parent[*cur] = *cur;
	printf("CURRENT ROOM = %d\n", bot.curRoom);
	while (*cur > 0 && cur < &worker[fieldSize]) {
		target = -1;

		// check all directions
		for (int i = North; i <= West; i++) {
			neighbor = findNeighbor(*cur, i); // find neighbor
			if (neighbor >= 0) { // valid neighbor
				if (parent[neighbor] < 0) {
					printf("neighbor %d\n", neighbor);
					parent[neighbor] = *cur; // neighbor came from cur
					*next++ = neighbor; // add neighbor to worker array
				}

				// if neighbor is unvisited, return it
				if (!field[neighbor].visited && target == -1) {
					target = neighbor;
					return target;
				}
			}
		}

		cur++; // advance worker


		if (target != -1) return target; // if it found a target tile, return it
	}
	return -1; // could not find any unvisited tile
}

//
int move2Tile(int cur, int target) {
	int i;
	int neighbor;
	int tileColor = Normal;

	// Find what direction to move to
	for (i = North; i <= West; i++) {
		neighbor = findNeighbor(cur, i);
		if (neighbor == target) break;
	}

	// Move to neighbors until reached target
	if (neighbor != target) {
		printf("target = %d parent of target = %d\n", target, parent[target]);
		move2Tile(cur, parent[target]);

		cur = parent[target];
		//getTile(bot, cur);
		for (i = North; i <= West; i++) {
			neighbor = findNeighbor(cur, i);
			if (neighbor == target) break;
		}
	}
	printf("target = %d cur = %d\n", target, cur);
	printf("current direction %d turn to %d\n", bot.getDirection(), i);

	// Turn to the right direction
	if (bot.getDirection() != i) bot.turn(i);
	// Move forward to the tile
	if (bot.curRoom == 1)
		tileColor = bot.move(12);
	else if (bot.curRoom == 2)
		tileColor = bot.move(6);

	switch (tileColor) {
		case Hole: // Black Hole
			field[target].N = 1;
			field[target].E = 1;
			field[target].S = 1;
			field[target].W = 1;
			field[target].visited = 1;
			printf("hole hole hole\n");
			return cur;
		case Blue: // Room 1 -> Room 2
			if (bot.curRoom == 2) {
				//if (bot.curRoom == 1) bot.curRoom = 2;
				//else if (bot.curRoom == 2) bot.curRoom = 1;
				//printf("BLUE\n");
				break;
			}
			printf("entering ROOM 2 for the first time tile = %d\n", target);
			bot.delay(3000);
			bot.curRoom = 2;
			bot.tile.room2 = target;

			break;
	}

	return target;
}
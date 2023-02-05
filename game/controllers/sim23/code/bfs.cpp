#include "bfs.h"

int parent[fieldSize];
int worker[fieldSize];

int room = 1;

// Check if valid neighbor
int findNeighbor(unsigned int tile, int dir) {
	if (room == 1) {
		unsigned int neighbors[] = { tile - COLS * 2, tile + 2, tile + COLS * 2, tile - 2 };
		switch (dir) {
			case North:
				if ((field[tile].bits & (1 << North)) == 0 && // TL N
					(field[tile + 1].bits & (1 << North)) == 0 && // TR N
					(field[tile - COLS].bits & (1 << South)) == 0 && // BL S
					(field[tile - COLS + 1].bits & (1 << South)) == 0) // BR S
					return neighbors[dir];
				break;
			case East:
				if ((field[tile + 1].bits & (1 << East)) == 0 && // TR E
					(field[tile + COLS + 1].bits & (1 << East)) == 0 && // BR E
					(field[neighbors[East]].bits & (1 << West)) == 0 && // TL W
					(field[neighbors[East] + COLS].bits & (1 << West)) == 0) // BL W
					return neighbors[dir];
				break;
			case South:
				if ((field[tile + COLS].bits & (1 << South)) == 0 && // BL S
					(field[tile + COLS + 1].bits & (1 << South)) == 0 && // BR S
					(field[neighbors[South]].bits & (1 << North)) == 0 && // TL N
					(field[neighbors[South] + 1].bits & (1 << North)) == 0) // TR N
					return neighbors[dir];
				break;
			case West:
				if ((field[tile].bits & (1 << West)) == 0 && // TL W
					(field[tile + COLS].bits & (1 << West)) == 0 && // BL W 
					(field[tile - 1].bits & (1 << East)) == 0 && // TR E
					(field[tile - 1 + COLS].bits & (1 << East)) == 0) // BR E 
					return neighbors[dir];
				break;
		}
	}



	// ROOM 2 STUFF
	
	//unsigned int neighbors[] = { tile - COLS, tile + 1, tile + COLS, tile - 1 }; // tiles to the north, east, south, west

	//if ((field[tile].bits & (1 << dir)) == 0) { // check that there is no wall on the respective side of the current node
	//	return neighbors[dir]; // return neighbor
	//}
	

	// ROOM 2 STUFF.

	/*
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
	*/
	
	// ROOM 2 STUFF


	return -1; // Tile is not a neighbor
}

// Breadth First Search
int bfs(Bot *bot, unsigned int tile) {
	int *cur = &worker[0];
	int *next = cur + 1;
	int neighbor = -1;
	int target = -1;

	// if there's a neighbor directly in front of the robot, just keep going forward
	//if (bot->getLidar(2, 496) > 8 && bot->getLidar(2, 15) > 8) {
	//	neighbor = findNeighbor(tile, bot->getDirection());
	//	if (neighbor != -1 && !field[neighbor].visited) return neighbor;
	//}
	//neighbor = -1;
	
	// reset parent and worker arrays
	memset(worker, -1, fieldSize * sizeof(worker[0]));
	memset(parent, -1, fieldSize * sizeof(parent[0]));

	*cur = tile; // add first tile to worker array
	parent[*cur] = *cur;
	while (*cur > 0 && cur < &worker[fieldSize]) {
		target = -1;

		// check all directions
		for (int i = North; i <= West; i++) {	
			neighbor = findNeighbor(*cur, i); // find neighbor
			if (neighbor >= 0) { // valid neighbor
				if (parent[neighbor] < 0) {
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
int move2Tile(Bot *bot, unsigned int cur, unsigned int target) {
	int i;
	int neighbor;

	// Find what direction to move to
	for (i = North; i <= West; i++) {
		neighbor = findNeighbor(cur, i);
		if (neighbor == target) break;
	}

	// Move to neighbors until reached target
	if (neighbor != target) {
		//printf("target = %d parent of target = %d\n", target, parent[target]);
		move2Tile(bot, cur, parent[target]);
		
		cur = parent[target];
		//getTile(bot, cur);
		for (i = North; i <= West; i++) {
			neighbor = findNeighbor(cur, i);
			if (neighbor == target) break;
		}
	}

	// Turn to the right direction
	switch (i) {
	case North:
		while (bot->update() && bot->getDirection() != North) {
			if (bot->getDirection() - 2 > North) {
				bot->turn(90);
			}
			else {
				bot->turn(-90);
			}
		}
		break;
	case East:
		while (bot->update() && bot->getDirection() != East) {
			if (bot->getDirection() < East) {
				bot->turn(90);
			}
			else {
				bot->turn(-90);
			}
		}
		break;
	case West:
		while (bot->update() && bot->getDirection() != West) {
			
			if (bot->getDirection() + 2 >= West) {
				bot->turn(90);
			}
			else {
				bot->turn(-90);
			}
		}
		break;
	case South:
		while (bot->update() && bot->getDirection() != South) {
			if (bot->getDirection() < South) {
				bot->turn(90);
			}
			else {
				bot->turn(-90);
			}
		}
		break;
	}

	// Move forward to the tile
	if (bot->move(12) == 0) {
		// black hole
		field[target].N = field[target].E = field[target].S = field[target].W = field[target].visited = 1;
		return cur;
	}
	return target;
}

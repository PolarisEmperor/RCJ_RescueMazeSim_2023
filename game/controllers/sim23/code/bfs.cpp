#include "bfs.h"

int parent[fieldSize];
int worker[fieldSize];

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

	neighbor = -1;
	memset(worker, -1, fieldSize * sizeof(worker[0]));
	memset(parent, -1, fieldSize * sizeof(parent[0]));

	*cur = tile; // add first tile to worker array
	parent[*cur] = *cur;
	while (*cur > 0 && cur < &worker[fieldSize]) {
		
		target = -1;
		printf("cur %d\n", *cur);
		// check all directions
		for (int i = North; i <= West; i++) {	
			neighbor = findNeighbor(*cur, i); // find neighbor
			printf("%d neighbor = %d\n", i, neighbor);
			if (neighbor >= 0) { // valid neighbor
				//printf("neighbor = %d\n", neighbor);
				printf("parent of neighbor = %d\n", parent[neighbor]);
				if (parent[neighbor] < 0) {
					
					parent[neighbor] = *cur; // neighbor came from cur
					*next++ = neighbor; // add neighbor to worker array
				}

				// if neighbor is unvisited, return it
				if (!field[neighbor].visited && target == -1) {
					target = neighbor;
					//printf("target = %d\n", neighbor);
					//return neighbor;
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
	if (bot->move(6) == 0) {
		// black hole
		field[target].N = field[target].E = field[target].S = field[target].W = field[target].visited = 1;
		return cur;
	}
	return target;
}

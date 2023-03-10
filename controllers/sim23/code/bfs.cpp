#include "bfs.h"

int parent[fieldSize];
int worker[fieldSize];

// Check if valid neighbor
int findNeighbor(unsigned int tile, int dir) {
	// tiles to the north, east, south, west
	unsigned int neighbors[] = { tile - COLS, tile + 1, tile + COLS, tile - 1 };

	switch (dir) {
		case North:
			if ((field[tile].bits & (1 << North)) == 0 && (field[neighbors[East]].bits & (1 << North)) == 0 && (field[neighbors[North]].bits & (1 << East)) == 0)
				return neighbors[dir];
			break;
		case East:
			if ((field[neighbors[East]].bits & (1 << East)) == 0 && (field[neighbors[South] + 1].bits & (1 << East)) == 0 && (field[neighbors[East] + 1].bits & (1 << South)) == 0)
				return neighbors[dir];
			break;
		case South:
			if ((field[neighbors[South]].bits & (1 << South)) == 0 && (field[neighbors[South] + 1].bits & (1 << South)) == 0 && (field[neighbors[South] + COLS].bits & (1 << East)) == 0)
				return neighbors[dir];
			break;
		case West:
			if ((field[tile].bits & (1 << West)) == 0 && (field[neighbors[South]].bits & (1 << West)) == 0 && (field[neighbors[West]].bits & (1 << South)) == 0)
				return neighbors[dir];
			break;
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
	//printf("CURRENT ROOM = %d\n", bot.curRoom);
	while (*cur > 0 && cur < &worker[fieldSize]) {
		target = -1;

		// check all directions
		for (int i = North; i <= West; i++) {
			neighbor = findNeighbor(*cur, i); // find neighbor
			if (neighbor >= 0) { // valid neighbor
				if (parent[neighbor] < 0) {
					//printf("cur %d neighbor %d\n", *cur, neighbor);
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

// Move to the target tile
int move2Tile(int cur, int target) {
	double targetPos = 0; // Position of target tile
	double distFromWall = 5.5; // Minimum distance to wall
	int neighbor;
	int tileColor = Normal;
	int i;

	// Find what direction to move to
	for (i = North; i <= West; i++) {
		neighbor = findNeighbor(cur, i);
		if (neighbor == target) break;
	}

	// Move to neighbors until reached target
	if (neighbor != target) {
		//printf("target = %d parent of target = %d\n", target, parent[target]);
		move2Tile(cur, parent[target]);

		cur = parent[target];
		for (i = North; i <= West; i++) {
			neighbor = findNeighbor(cur, i);
			if (neighbor == target) break;
		}
	}
	//printf("target = %d cur = %d\n", target, cur);
	//printf("current direction %d turn to %d\n", bot.getDirection(), i);

	// Turn to the right direction
	if (bot.getDirection() != i) bot.turn(i);
	
	// Drive forward
	switch (bot.getDirection()) {
		case North:
			targetPos = bot.getPrevPos().y - 6.0 / 100;
			break;
		case East:
			targetPos = bot.getPrevPos().x + 6.0 / 100;
			break;
		case South:
			targetPos = bot.getPrevPos().y + 6.0 / 100;
			break;
		case West:
			targetPos = bot.getPrevPos().x - 6.0 / 100;
			break;
	}
	bot.stop();
	
	while (bot.update()) {
		if (field[cur].victimChecked == 0 && (checkVisualVictim(bot.camL) || checkVisualVictim(bot.camR))) {
			printf("found victim at tile %d\n", cur);
			//field[cur].victimChecked = 1;
		}

		tileColor = bot.getTileColor(bot.camB->getWidth() / 2, 25);
		if (tileColor == Hole) { // bottom cam sees black
			bot.stop();

			// back up to prev pos
			bot.speed(-6, -6);
			printf("theres a black hole, backing up\n");

			if (bot.getDirection() == North || bot.getDirection() == South)
				if (bot.getPos().y < bot.getPrevPos().y) while (bot.update() && bot.getPos().y < bot.getPrevPos().y);
				else while (bot.update() && bot.getPos().y > bot.getPrevPos().y);
			else
				if (bot.getPos().x < bot.getPrevPos().x) while (bot.update() && bot.getPos().x < bot.getPrevPos().x);
				else while (bot.update() && bot.getPos().x > bot.getPrevPos().x);

			bot.stop();
			// Set all wall bits of tile
			field[target].N = field[target].E = field[target].S = field[target].W = field[target].visited = 1;
			field[target + 1].N = field[target + 1].E = field[target + 1].S = field[target + 1].W = field[target + 1].visited = 1;
			field[target + COLS].N = field[target + COLS].E = field[target + COLS].S = field[target + COLS].W = field[target + COLS].visited = 1;
			field[target + COLS + 1].N = field[target + COLS + 1].E = field[target + COLS + 1].S = field[target + COLS + 1].W = field[target + COLS + 1].visited = 1;
			return cur;
		}

		if (bot.getDirection() == North && bot.getPos().y < targetPos) break;
		if (bot.getDirection() == South && bot.getPos().y > targetPos) break;
		if (bot.getDirection() == East && bot.getPos().x > targetPos) break;
		if (bot.getDirection() == West && bot.getPos().x < targetPos) break;
		if (bot.getLidar(3, 0) < distFromWall) break;

		// IMU Straighten
		switch (bot.getDirection()) {
			case North:
				if (bot.getAngle() < 0) bot.speed(5, 6);
				else if (bot.getAngle() > 0) bot.speed(6, 5);
				else bot.speed(6, 6);
				break;
			case East:
				if (bot.getAngle() < -1.57) bot.speed(5, 6);
				else if (bot.getAngle() > -1.57) bot.speed(6, 5);
				else bot.speed(6, 6);
				break;
			case South:
				if (bot.getAngle() > 0 && bot.getAngle() < 3.14) bot.speed(5, 6);
				else if (bot.getAngle() < 0 && bot.getAngle() > -3.14) bot.speed(6, 5);
				else bot.speed(6, 6);
				break;
			case West:
				if (bot.getAngle() < 1.57) bot.speed(5, 6);
				else if (bot.getAngle() > 1.57) bot.speed(6, 5);
				else bot.speed(6, 6);
				break;
		}
	}
	bot.stop();

	if (tileColor == Swamp) printf("SWAMP\n");
	if (tileColor == Checkpoint) printf("CHECKPOINT\n");
	if (tileColor == Blue) printf("BLUE\n");
	if (tileColor == Purple) printf("PURPLE\n");
	if (tileColor == Red) printf("RED\n");

	switch (tileColor) {
		case Blue: // Room 1 -> Room 2
			if (bot.curRoom == 2 && target == bot.blueTile) {
				printf("room 2 -> 1\n");
				bot.delay(3000);
				bot.curRoom = 1;
			}
			else if (bot.curRoom == 1 && target == bot.blueTile) {
				printf("room 1 -> 2\n");
				bot.delay(3000);
				bot.curRoom = 2;
			}
			else if (target % 2 == 0 && (int(target / 10 * 10) % (COLS * 2) == 0 || (int(target / 10 * 10) + 10) % (COLS * 2) == 0) && bot.blueTile < 0) {
				printf("entering ROOM 2 for the first time tile = %d\n", target);
				bot.delay(3000);
				bot.curRoom = 2;
				bot.blueTile = target;
			}
			break;
	}

	bot.updatePrevPos();
	return target;
}
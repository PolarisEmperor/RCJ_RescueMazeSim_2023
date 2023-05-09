#include "bfs.h"

int parent[fieldSize];
int worker[fieldSize];
int diffX = 0, diffY = 0;

// Check if valid neighbor
int findNeighbor(unsigned int tile, int dir) {
	// tiles to the north, east, south, west
	unsigned int neighbors[] = { tile - COLS, tile + 1, tile + COLS, tile - 1 };

	switch (dir) {
		case North:
			if ((field[tile].bits & (1 << North)) == 0 && (field[neighbors[East]].bits & (1 << North)) == 0 && (field[neighbors[North]].bits & (1 << East)) == 0) {
				return neighbors[dir];
			}
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
			targetPos = bot.getTargetPos(diffX, diffY -= 1).y;
			//targetPos = bot.getPrevPos().y - 6.0 / 100;
			break;
		case East:
			targetPos = bot.getTargetPos(diffX += 1, diffY).x;
			//targetPos = bot.getPrevPos().x + 6.0 / 100;
			break;
		case South:
			targetPos = bot.getTargetPos(diffX, diffY += 1).y;
			//targetPos = bot.getPrevPos().y + 6.0 / 100;
			break;
		case West:
			targetPos = bot.getTargetPos(diffX -= 1, diffY).x;
			//targetPos = bot.getPrevPos().x - 6.0 / 100;
			break;
	}
	//printf("targetPos: %f\n", targetPos);
	bot.stop();
	
	while (bot.update()) {
		if (bot.getLidar(3, 127) < 6 && !field[cur].victimChecked) {
			if (checkVisualVictim(bot.camR) ) {
				printf("found victim at tile %d\n", cur);
			}
		}
		if (bot.getLidar(3, 383) < 6 && !field[cur].victimChecked) {
			if (checkVisualVictim(bot.camL)) {
				printf("found victim at tile %d\n", cur);
			}
		}

		tileColor = bot.getTileColor(bot.camB->getWidth() / 2, 0);
		int left = bot.getTileColor(bot.camB->getWidth() / 4, 0);
		int right = bot.getTileColor(bot.camB->getWidth() * 3 / 4, 0);
		//printf("tile color %d %d\n", left, right);
		
		if (left == Hole || right == Hole) { // bottom cam sees black
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

			//						tl					tr			bl			br
			int blackHole[4] = { target - COLS, target - COLS + 1, target, target + 1 };

			switch (bot.getDirection()) {
				case North:
					diffY += 1;
					if (left == Hole && right != Hole) { // Left
						for (int i = 0; i < 4; i++) {
							blackHole[i]--;
						}
					}
					else if (left != Hole && right == Hole) { // Right
						for (int i = 0; i < 4; i++) {
							blackHole[i]++;
						}
					}
					break;
				case East:
					diffX -= 1;
					for (int i = 0; i < 4; i++) {
						blackHole[i] += COLS + 1;
					}
					if (left == Hole && right != Hole) { // Left
						for (int i = 0; i < 4; i++) {
							blackHole[i] -= COLS;
						}
					}
					else if (left != Hole && right == Hole) { // Right
						for (int i = 0; i < 4; i++) {
							blackHole[i] += COLS;
						}
					}
					break;
				case South:
					diffY -= 1;
					for (int i = 0; i < 4; i++) {
						blackHole[i] += COLS * 2;
					}
					if (left == Hole && right != Hole) { // Left
						for (int i = 0; i < 4; i++) {
							blackHole[i] += 1;
						}
					}
					else if (left != Hole && right == Hole) { // Right
						for (int i = 0; i < 4; i++) {
							blackHole[i] -= 1;
						}
					}
					break;
				case West:
					diffX += 1;
					for (int i = 0; i < 4; i++) {
						blackHole[i] += COLS - 1;
					}
					if (left == Hole && right != Hole) { // Left
						for (int i = 0; i < 4; i++) {
							blackHole[i] += COLS;
						}
					}
					else if (left != Hole && right == Hole) { // Right
						for (int i = 0; i < 4; i++) {
							blackHole[i] -= COLS;
						}
					}
					break;
			}

			for (int i = 0; i < 4; i++) {
				setWalls(blackHole[i], 1, 1, 1, 1);
				field[blackHole[i]].visited = 1;
				field[blackHole[i]].color = Hole;
			}

			printf("there was a black hole. robot on tile %d\n", cur);
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
				//bot.delay(3000);
				bot.curRoom = 1;
			}
			else if (bot.curRoom == 1 && target == bot.blueTile) {
				printf("room 1 -> 2\n");
				//bot.delay(3000);
				bot.curRoom = 2;
			}
			else if (target % 2 == 0 && (int(target / 10 * 10) % (COLS * 2) == 0 || (int(target / 10 * 10) - 20) % (COLS * 2) == 0) && bot.blueTile < 0) {
				printf("entering ROOM 2 for the first time tile = %d\n", target);
				bot.delay(3000);
				bot.curRoom = 2;
				bot.blueTile = target;
				field[target].color = Blue;
				printf("tile %d color %d\n", target, field[target].color);
			}
			break;
		case Purple: // Room 2 -> Room 3
			if (bot.curRoom == 3 && target == bot.purpleTile) {
				printf("room 3 -> 2\n");
				//bot.delay(3000);
				bot.curRoom = 2;
			}
			else if (bot.curRoom == 2 && target == bot.purpleTile) {
				printf("room 2 -> 3\n");
				//bot.delay(3000);
				bot.curRoom = 3;
			}
			else if (target % 2 == 0 && (int(target / 10 * 10) % (COLS * 2) == 0 || (int(target / 10 * 10) - 20) % (COLS * 2) == 0) && bot.purpleTile < 0) {
				printf("entering ROOM 3 for the first time tile = %d\n", target);
				//bot.delay(3000);
				bot.curRoom = 3;
				bot.purpleTile = target;
				field[target].color = Purple;
			}

			break;
		default:
			if (tileColor != 0 && target % 2 == 0 && ((int(target / 10 * 10) - 10) % (COLS * 2) == 0 || (int(target / 10 * 10) - 20) % (COLS * 2) == 0)) {
				field[target].color = tileColor;
			}
			break;
	}

	bot.updatePrevPos();
	return target;
}
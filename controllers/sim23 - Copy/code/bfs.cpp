#include "bfs.h"

int parent[fieldSize];
int worker[fieldSize];
int diffX = 0, diffY = 0;

// Check if valid neighbor
int findNeighbor(unsigned int tile, int dir) {
	// tiles to the north, east, south, west
	unsigned int neighbors[] = { tile - COLS, tile + 1, tile + COLS, tile - 1 };

	// out of bounds checking
	if (tile > fieldSize - 1 || tile < 0) return -2;
	for (int i = 0; i < 4; i++) {
		if (neighbors[i] < 0 || neighbors[i] > fieldSize - 1)
			return -2;
	}

	switch (dir) {
		case North:
			if (field[neighbors[North]].color != Hole && field[neighbors[North] + 1].color != Hole && (field[tile].bits & (1 << North)) == 0 && (field[neighbors[East]].bits & (1 << North)) == 0 && (field[neighbors[North]].bits & (1 << East)) == 0) {
				//if (field[neighbors[North]].color >= Red && bot.room4done) break;
				return neighbors[dir];
			}
			break;
		case East:
			if (field[neighbors[East] + 1].color != Hole && field[neighbors[East] + 1 + COLS].color != Hole && (field[neighbors[East]].bits & (1 << East)) == 0 && (field[neighbors[South] + 1].bits & (1 << East)) == 0 && (field[neighbors[East] + 1].bits & (1 << South)) == 0) {
				//if (field[neighbors[East]].color >= Red && bot.room4done) break;
				return neighbors[dir];
			}
				
			break;
		case South:
			if (field[neighbors[South] + COLS].color != Hole && field[neighbors[South] + COLS + 1].color != Hole && (field[neighbors[South]].bits & (1 << South)) == 0 && (field[neighbors[South] + 1].bits & (1 << South)) == 0 && (field[neighbors[South] + COLS].bits & (1 << East)) == 0) {
				//if (field[neighbors[South]].color >= Red && bot.room4done) break;
				return neighbors[dir];
			}
				
			break;
		case West:
			if (field[neighbors[West]].color != Hole && field[neighbors[West] + COLS].color != Hole && (field[tile].bits & (1 << West)) == 0 && (field[neighbors[South]].bits & (1 << West)) == 0 && (field[neighbors[West]].bits & (1 << South)) == 0) {
				//if (field[neighbors[West]].color >= Red && bot.room4done) break;
				return neighbors[dir];
			}
				
			break;
	}
	return -1; // Tile is not a neighbor
}

// Breadth First Search
int bfs(int tile) {
	int curIdx = 0;
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
			//printf("in loop %d\n", i);
			neighbor = findNeighbor(*cur, i); // find neighbor
			if (neighbor >= 0) { // valid neighbor
				if (parent[neighbor] < 0) {
					//printf("cur %d neighbor %d parent of neighbor %d\n", *cur, neighbor, parent[neighbor]);
					
					parent[neighbor] = *cur; // neighbor came from cur
					*next++ = neighbor; // add neighbor to worker array
				}

				// if neighbor is unvisited, return it
				if (!field[neighbor].visited && target == -1) {
					target = neighbor;
					return target;
				}
			}
			else if (neighbor == -2) {
				printf("BFS FAILED\n");
				return -2;
			}

		}
		for (int i = 0; worker[i] >= 0; i++) {
			//printf("%d %d\n", i, worker[i]);
		}
		cur = &worker[++curIdx]; // advance worker
		//printf("cur = %d\n", *cur);

		if (target != -1) return target; // if it found a target tile, return it
	}
	return -1; // could not find any unvisited tile
}

int gohome(int tile) {
	printf("go home\n");
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
					printf("cur %d neighbor %d\n", *cur, neighbor);
					parent[neighbor] = *cur; // neighbor came from cur
					*next++ = neighbor; // add neighbor to worker array
				}
				if (neighbor == -2) {
					printf("BFS FAILED\n");
					char message = 'E';
					bot.emitter->send(&message, 1);

					while (bot.robot->step(bot.robot->getBasicTimeStep()) != 1);
					return -2;
				}
				// if neighbor is unvisited, return it
				/*if (!field[neighbor].visited && target == -1) {
					target = neighbor;
					return target;
				}*/
				if (neighbor == bot.startTile) return neighbor;
			}
		}
		cur++; // advance worker

		if (target != -1) return target; // if it found a target tile, return it
	}
	return -1; // could not find any unvisited tile
}

bool oppDir(int curDir, int targetDir) {
	switch (curDir) {
		case North:
			if (targetDir == South) return true;
			break;
		case East:
			if (targetDir == West) return true;
			break;
		case South:
			if (targetDir == North) return true;
			break;
		case West:
			if (targetDir == East) return true;
			break;
	}
	return false;
}

// Move to the target tile
int move2Tile(int cur, int target) {
	double targetPos = 0; // Position of target tile
	double distFromWall = 5.5; // Minimum distance to wall
	int neighbor;
	int tileColor = Normal;
	int i;
	bool opp = 0;

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
		for (i = North; i <= West; i++) {
			neighbor = findNeighbor(cur, i);
			if (neighbor == target) break;
		}
	}

	// Turn to the right direction
	if (bot.getDirection() != i) bot.turn(i);

	// Drive forward
	/*switch (bot.getDirection()) {
		case North:
			targetPos = bot.getTargetPos(diffX, diffY -= 1).y;
			break;
		case East:
			targetPos = bot.getTargetPos(diffX += 1, diffY).x;
			break;
		case South:
			targetPos = bot.getTargetPos(diffX, diffY += 1).y;
			break;
		case West:
			targetPos = bot.getTargetPos(diffX -= 1, diffY).x;
			break;
	}*/
	if (bot.getDirection() % 2 == 0) targetPos = bot.tileToCoords(target).y;
	else targetPos = bot.tileToCoords(target).x;
	
	//bot.stop();

	while (bot.update()) {
		// Check for victims
		if (bot.getLidarPoint(3, 127) < 7 && !field[cur].victimChecked) {
			char victim = checkVisualVictim(bot.camR);
			if (victim > 0) {
				int wall = bot.getDirection() + 1;
				if (wall >= 4) wall -= 4;
				printf("found right victim at tile %d %d %d\n", cur, wall, victim);
				mapVictim(cur, wall, victim);
			}
		}
		if (bot.getLidarPoint(3, 383) < 7 && !field[cur].victimChecked) {
			char victim = checkVisualVictim(bot.camL);
			if (victim > 0) {
				int wall = bot.getDirection() - 1;
				if (wall < 0) wall += 4;
				printf("found left victim at tile %d %d %d\n", cur, wall, victim);
				mapVictim(cur, wall, victim);
			}
		}

		tileColor = bot.getTileColor(bot.camB->getWidth() / 2, 0);
		int left = bot.getTileColor(bot.camB->getWidth() / 4, 0);
		int right = bot.getTileColor(bot.camB->getWidth() * 3 / 4, 0);

		if (left == Hole || right == Hole) { // bottom cam sees black
			bot.stop();

			// get the walls of the black hole tile
			if (left == Hole && right == Hole) getBlackHole(cur);

			// back up to prev pos
			bot.speed(-6, -6);
			printf("theres a black hole, backing up to %f %f\n", bot.getPrevPos().x, bot.getPrevPos().y);

			/*if (bot.getDirection() == North || bot.getDirection() == South)
				if (bot.getPos().y < bot.getPrevPos().y) while (bot.update() && bot.getPos().y < bot.getPrevPos().y);
				else while (bot.update() && bot.getPos().y > bot.getPrevPos().y);
			else
				if (bot.getPos().x < bot.getPrevPos().x) while (bot.update() && bot.getPos().x < bot.getPrevPos().x);
				else while (bot.update() && bot.getPos().x > bot.getPrevPos().x);*/

			bot.move(-0.2);
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
				//setWalls(blackHole[i], 1, 1, 1, 1);
				printf("hole %d\n", blackHole[i]);
				//field[blackHole[i]].visited = 1;
				field[blackHole[i]].color = Hole;
			}

			printf("there was a black hole. robot on tile %d\n", cur);
			editMapTile(blackHole[0]);
			return cur;
		}
		if (obstacle(cur)) {
			bot.stop();

			// back up to prev pos
			bot.speed(-6, -6);
			printf("OBSTACLE!!!\n");

			if (bot.getDirection() == North || bot.getDirection() == South)
				if (bot.getPos().y < bot.getPrevPos().y) while (bot.update() && bot.getPos().y < bot.getPrevPos().y);
				else while (bot.update() && bot.getPos().y > bot.getPrevPos().y);
			else
				if (bot.getPos().x < bot.getPrevPos().x) while (bot.update() && bot.getPos().x < bot.getPrevPos().x);
				else while (bot.update() && bot.getPos().x > bot.getPrevPos().x);

			bot.stop();
			printf("there was an obstacle. robot on tile %d\n", bot.curTile);
			switch (bot.getDirection()) {
				case North:
					diffY += 1;
					break;
				case East:
					diffX -= 1;
					break;
				case South:
					diffY -= 1;
					break;
				case West:
					diffX += 1;
					break;
			}
			return bot.curTile;
		}

		// reached target pos
		if (bot.getDirection() == North && bot.getPos().y < targetPos) { 
			//printf("north\n"); 
			break; 
		}
		if (bot.getDirection() == South && bot.getPos().y > targetPos) { 
			//printf("east\n"); 
			break; 
		}
		if (bot.getDirection() == East && bot.getPos().x > targetPos)  { 
			//printf("south\n"); 
			break; 
		}
		if (bot.getDirection() == West && bot.getPos().x < targetPos)  { 
			//printf("west\n"); 
			break; 
		}
		//if (bot.getLidarPoint(3, 0) < distFromWall) {
		//	//bot.move(-0.2);
		//	break;
		//}
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

	int curRow = target / ROWS, curCol = target % COLS;

	switch (tileColor) {
		case Blue: // Room 1 -> Room 2
			printf("%d %d %d\n", target, cur, bot.blueTile);
			if (bot.curRoom == 2 && target == bot.blueTile) {
				printf("room 2 -> 1\n");
				bot.curRoom = 1;
			}
			else if (bot.curRoom == 1 && target == bot.blueTile) {
				printf("room 1 -> 2\n");
				bot.curRoom = 2;
			}
			else if (target % 2 == 0 && (curRow % 2 == 0 && curCol % 2 == 0) && bot.blueTile < 0) {
				printf("entering ROOM 2 for the first time tile = %d\n", target);
				bot.blueTile = target;
				field[target].color = Blue;
				bot.curRoom = 2;
				printf("tile %d color %d\n", target, field[target].color);
			}
			break;
		case Purple: // Room 2 -> Room 3
			if (bot.curRoom == 3 && target == bot.purpleTile) {
				printf("room 3 -> 2\n");
				bot.curRoom = 2;
			}
			else if (bot.curRoom == 2 && target == bot.purpleTile) {
				printf("room 2 -> 3\n");
				bot.curRoom = 3;
			}
			else if (target % 2 == 0 && ((curRow % 2 == 0 && curCol % 2 == 0)) && bot.purpleTile < 0) {
				printf("entering ROOM 3 for the first time tile = %d\n", target);
				bot.curRoom = 3;
				bot.purpleTile = target;
				field[target].color = Purple;
			}
			break;
		case Green: // Room 1 -> Room 4
			if (bot.curRoom == 4 && target == bot.greenTile) {
				//printf("room 4 -> 1\n");
				//bot.curRoom = 1;
			}
			else if (bot.curRoom == 1 && target == bot.greenTile) {
				//printf("room 1 -> 4\n");
				//bot.curRoom = 4;
			}
			else if (target % 2 == 0 && ((curRow % 2 == 0 && curCol % 2 == 0)) && bot.greenTile < 0) {
				printf("entering ROOM 4 for the first time tile = %d\n", target);
				//if (bot.curRoom == 4) bot.curRoom = 1;
				//else if (bot.curRoom == 1) bot.curRoom = 4;
				if (!bot.room4done) bot.curRoom = 4;
				bot.greenTile = target;
				field[target].color = Green;
				editMapTile(target);
			}

			break;
		case Red: // Room 3 -> Room 4
			if (bot.curRoom == 4 && target == bot.redTile) {
				printf("room 4 -> 3\n");
				//bot.curRoom = 3;
			}
			else if (bot.curRoom == 3 && target == bot.redTile) {
				printf("room 3 -> 4\n");
				//bot.curRoom = 4;
			}
			else if (target % 2 == 0 && ((curRow % 2 == 0 && curCol % 2 == 0)) && bot.redTile < 0) {
				printf("entering ROOM 4 for the first time tile = %d\n", target);
				bot.redTile = target;
				field[target].color = Red;
				if (!bot.room4done) bot.curRoom = 4;
				editMapTile(target);
			}
			break;
		case Checkpoint:
			bot.checkpointTile = target;
			bot.checkpointRoom = bot.curRoom;
			printf("checkpoint %d\n", bot.checkpointTile);
		default:
			if (tileColor != 0 && target % 2 == 0 && ((curRow % 2 == 0 && curCol % 2 == 0))) {
				field[target].color = tileColor;
			}
			break;
	}
	if (bot.getGameTime() < 5) {
		mapBonus();
	}
	bot.updatePrevPos();
	return target;
}
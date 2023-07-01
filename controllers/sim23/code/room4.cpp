#include "room4.h"

void LOP() {
	char message[1] = { 'L' }; // message = 'L' to activate lack of progress
	bot.emitter->send(message, sizeof(message)); // Send out the message array. Note that the 2nd parameter must be the size of the message
	bot.robot->step(bot.robot->getBasicTimeStep());
}

bool compareCoords(double x1, double y1, double x2, double y2) {
	printf("X: %f Y: %f\n", fabs(x1) - fabs(x2), fabs(y1) - fabs(y2));
	if (fabs(x1) - fabs(x2) > 0.05 || fabs(x1) - fabs(x2) < -0.05) {
		return 0;
	}
	if (fabs(y1) - fabs(y2) > 0.05 || fabs(y1) - fabs(y2) < -0.05) {
		return 0;
	}

	return 1;
}

int coordToTile(double x, double y) {
	int tile = bot.startTile;
	if (bot.startPos.x < x) {
		while (!compareCoords(x, y, bot.startPos.x, bot.startPos.y)) {
			x -= 0.06;
			tile++;
		}
	}
	else {
		while (!compareCoords(x, y, bot.startPos.x, bot.startPos.y)) {
			x += 0.06;
			tile--;
		}
	}
	if (bot.startPos.y < y) {
		while (!compareCoords(x, y, bot.startPos.x, bot.startPos.y)) {
			y -= 0.06;
			tile+=COLS;
		}
	}
	else {
		while (!compareCoords(x, y, bot.startPos.x, bot.startPos.y)) {
			y += 0.06;
			tile-=COLS;
		}
	}
	return tile;
}

void doRoom4() {
	double x = bot.getPos().x;
	double y = bot.getPos().y;
	double checkpointX, checkpointY;

	int startDir = bot.getDirection();
	int color = bot.getTileColor(0, 0);
	int wallTraceColor;
	int directionEntered = 0;
	bool LOProom4 = false;
	bool proceed = false;

	printf("start dir %d\n", startDir);
	printf("color %d\n", color);
	//bot.delay(1000);

	// there should be walls surroundng the green/red tile.
	// look for the checkpoint tile
	// try north
	if (bot.getLidarPoint(3, 481) > 9 && bot.getLidarPoint(3, 30) > 9) {
		printf("try north\n");
		bot.move(3);
		// if not checkpoint, backup and try another dir
		if (bot.getTileColor(0, 0) == Checkpoint) {
			proceed = true;
		}
		else {
			bot.move(-3);
		}
	}
	else {
		switch (bot.getDirection()) {
			case North:
				setWalls(bot.curTile, 1, 0, 0, 0);
				setWalls(bot.curTile + 1, 1, 0, 0, 0);
				break;
			case East:
				setWalls(bot.curTile + 1 + COLS, 0, 1, 0, 0);
				setWalls(bot.curTile + 1, 0, 1, 0, 0);
				break;
			case South:
				setWalls(bot.curTile + COLS, 0, 0, 1, 0);
				setWalls(bot.curTile + 1 + COLS, 0, 0, 1, 0);
				break;
			case West:
				setWalls(bot.curTile, 0, 0, 0, 1);
				setWalls(bot.curTile + COLS, 0, 0, 0, 1);
				break;
		}
	}

	// try right side
	if (bot.getLidarPoint(3, 157) > 9 && bot.getLidarPoint(3, 225) > 9 && !proceed) {
		printf("try right\n");
		bot.turn(bot.getDirection() + 1); // turn
		bot.move(3);
		if (bot.getTileColor(0, 0) == Checkpoint) {
			proceed = true;
		}
		else {
			bot.move(-3);
			bot.turn(bot.getDirection() - 1);
		}
	}
	else {
		switch (bot.getDirection()) {
			case North:
				setWalls(bot.curTile + 1 + COLS, 0, 1, 0, 0);
				setWalls(bot.curTile + 1, 0, 1, 0, 0);
				break;
			case East:
				setWalls(bot.curTile + COLS, 0, 0, 1, 0);
				setWalls(bot.curTile + 1 + COLS, 0, 0, 1, 0);
				break;
			case South:
				setWalls(bot.curTile, 0, 0, 0, 1);
				setWalls(bot.curTile + COLS, 0, 0, 0, 1);
				break;
			case West:
				setWalls(bot.curTile, 1, 0, 0, 0);
				setWalls(bot.curTile + 1, 1, 0, 0, 0);
				break;
		}
	}

	// try left side
	if (bot.getLidarPoint(3, 353) > 9 && bot.getLidarPoint(3, 413) > 9 && !proceed) {
		printf("try left\n");
		bot.turn(bot.getDirection() - 1); // turn
		bot.move(3);
		if (bot.getTileColor(0, 0) == Checkpoint) {
			proceed = true;
		}
		else {
			bot.move(-3);
			bot.turn(bot.getDirection() + 1);
		}
	}
	else {
		switch (bot.getDirection()) {
			case North:
				setWalls(bot.curTile, 0, 0, 0, 1);
				setWalls(bot.curTile + COLS, 0, 0, 0, 1);
				
				break;
			case East:
				setWalls(bot.curTile, 1, 0, 0, 0);
				setWalls(bot.curTile + 1, 1, 0, 0, 0);
				
				break;
			case South:
				setWalls(bot.curTile + 1 + COLS, 0, 1, 0, 0);
				setWalls(bot.curTile + 1, 0, 1, 0, 0);
				
				
				break;
			case West:
				setWalls(bot.curTile + COLS, 0, 0, 1, 0);
				setWalls(bot.curTile + 1 + COLS, 0, 0, 1, 0);
				break;
		}
	}

	// valid room 4, proceed with navigation
	if (proceed) {
		bot.move(9);
		printf("on checkpoint\n");
		checkpointX = bot.getPos().x;
		checkpointY = bot.getPos().y;
		directionEntered = bot.getDirection();
		// bot is on checkpoint tile, set as checkpoint
		
		// trace
		while (bot.update() && !LOProom4) {
			printf("first\n");
			// lack of progress called
			if (bot.checkLOPemitter()) {
				LOProom4 = true;
				break;
			}
			// wall trace
			wallTraceColor = room4_wallTrace();
			printf("color = %d\n", wallTraceColor);
			
			// if it sees a green or red tile, break
			if (color == wallTraceColor) break;
			if (color == Green && wallTraceColor == Red) break;
			if (color == Red && wallTraceColor == Green) break;
		}

		// not at entrance tile, turn around
		if (wallTraceColor != color && !LOProom4) {
			printf("turn around!\n");
			switch (bot.roundAngle()) {
				case 0: case 1: bot.turn(South); break;
				case 2: case 3: bot.turn(West); break;
				case 4: case 5: bot.turn(North); break;
				case 6: case 7: bot.turn(East); break;
			}

			// wall trace until it's at the entrance
			while (bot.update()) {
				if (room4_wallTrace() == color) {
					break;
				}
				printf("second\n");
				if (bot.checkLOPemitter()) {
					LOProom4 = true;
					break;
				}
			}
		}

		if (!LOProom4) {
			// finished room 4
			bot.turn(East);
			printf("%f %f %f %f\n", bot.getPos().x, bot.getPos().y, x, y);
			while (bot.update() && bot.getPos().x < x) {
				bot.speed(5, 5);
			}
			bot.stop();
			while (bot.update() && bot.getPos().x > x) {
				bot.speed(-5, -5);
			}
			bot.stop();
			bot.turn(North);
			while (bot.update() && bot.getPos().y < y) {
				bot.speed(-5, -5);
			}
			bot.stop();
			while (bot.update() && bot.getPos().y > y) {
				bot.speed(5, 5);
			}
			bot.stop();

			bot.stop();
			bot.room4done = true;

			if (color == Green) {
				bot.curRoom = 1;
				bot.curTile = bot.greenTile;
			}
			else if (color == Red) {
				bot.curRoom = 3;
				bot.curTile = bot.redTile;
			}
			printf("DIRECTION ENTERED %d\n", directionEntered);
			//getTile(bot.curTile);

			switch (directionEntered) {
				case North:
					setWalls(bot.curTile, 1, 0, 0, 0);
					setWalls(bot.curTile + 1, 1, 0, 0, 0);
					break;
				case East:
					setWalls(bot.curTile + 1, 0, 1, 0, 0);
					setWalls(bot.curTile + COLS + 1, 0, 1, 0, 0);
					break;
				case South:
					setWalls(bot.curTile + COLS, 0, 0, 1, 0);
					setWalls(bot.curTile + COLS + 1, 0, 0, 1, 0);
					break;
				case West:
					setWalls(bot.curTile, 0, 0, 0, 1);
					setWalls(bot.curTile + COLS, 0, 0, 0, 1);
					break;
			}
			switch (startDir) {
				case North:
					setWalls(bot.curTile, 1, 0, 0, 0);
					setWalls(bot.curTile + 1, 1, 0, 0, 0);
					break;
				case East:
					setWalls(bot.curTile + 1, 0, 1, 0, 0);
					setWalls(bot.curTile + COLS + 1, 0, 1, 0, 0);
					break;
				case South:
					setWalls(bot.curTile + COLS, 0, 0, 1, 0);
					setWalls(bot.curTile + COLS + 1, 0, 0, 1, 0);
					break;
				case West:
					setWalls(bot.curTile, 0, 0, 0, 1);
					setWalls(bot.curTile + COLS, 0, 0, 0, 1);
					break;
			}

			field[bot.curTile].visited = 1;
			field[bot.curTile + 1].visited = 1;
			field[bot.curTile + COLS].visited = 1;
			field[bot.curTile + COLS + 1].visited = 1;
		}
	}
	else {
		printf("invalid room 4!\n");
		switch (bot.getDirection()) {
			case North:
				setWalls(bot.curTile, 1, 0, 0, 0);
				setWalls(bot.curTile + 1, 1, 0, 0, 0);
				break;
			case East:
				setWalls(bot.curTile + 1, 0, 1, 0, 0);
				setWalls(bot.curTile + COLS + 1, 0, 1, 0, 0);
				break;
			case South:
				setWalls(bot.curTile + COLS, 0, 0, 1, 0);
				setWalls(bot.curTile + COLS + 1, 0, 0, 1, 0);
				break;
			case West:
				setWalls(bot.curTile, 0, 0, 0, 1);
				setWalls(bot.curTile + COLS, 0, 0, 0, 1);
				break;
		}
		field[bot.curTile].visited = 1;
		field[bot.curTile + 1].visited = 1;
		field[bot.curTile + COLS].visited = 1;
		field[bot.curTile + COLS + 1].visited = 1;
		bot.room4done = true;
		if (color == Green) {
			bot.curRoom = 1;
			bot.curTile = bot.greenTile;
		}
		else if (color == Red) {
			bot.curRoom = 3;
			bot.curTile = bot.redTile;
		}
	}
	// lack of progress in room 4
	while (bot.update() && LOProom4) {
		printf("LACK OF PROGRESS IN ROOM 4!!!\n");
		printf("%f %f %f %f\n", bot.getPos().x, bot.getPos().y, checkpointX, checkpointY);
		// teleported to entrance checkpoint
		if (compareCoords(bot.getPos().x, bot.getPos().y, checkpointX, checkpointY)) {
			printf("at entrance checkpoint\n");
			
			bot.room4done = true;
			
			bot.turn(directionEntered - 2);
			printf("driving forward\n");
			bot.move(12);
			bot.stop();
			if (bot.getTileColor(0, 0) == Green) {
				bot.curRoom = 1;
				bot.curTile = bot.greenTile;
			}
			else if (bot.getTileColor(0, 0) == Red) {
				bot.curRoom = 3;
				bot.curTile = bot.redTile;
			}
			printf("DIRECTION ENTERED %d\n", startDir);
			switch (directionEntered) {
				case North:
					setWalls(bot.curTile, 1, 0, 0, 0);
					setWalls(bot.curTile + 1, 1, 0, 0, 0);
					break;
				case East:
					setWalls(bot.curTile + 1, 0, 1, 0, 0);
					setWalls(bot.curTile + COLS + 1, 0, 1, 0, 0);
					break;
				case South:
					setWalls(bot.curTile + COLS, 0, 0, 1, 0);
					setWalls(bot.curTile + COLS + 1, 0, 0, 1, 0);
					break;
				case West:
					setWalls(bot.curTile, 0, 0, 0, 1);
					setWalls(bot.curTile + COLS, 0, 0, 0, 1);
					break;
			}
			switch (startDir) {
				case North:
					setWalls(bot.curTile, 1, 0, 0, 0);
					setWalls(bot.curTile + 1, 1, 0, 0, 0);
					break;
				case East:
					setWalls(bot.curTile + 1, 0, 1, 0, 0);
					setWalls(bot.curTile + COLS + 1, 0, 1, 0, 0);
					break;
				case South:
					setWalls(bot.curTile + COLS, 0, 0, 1, 0);
					setWalls(bot.curTile + COLS + 1, 0, 0, 1, 0);
					break;
				case West:
					setWalls(bot.curTile, 0, 0, 0, 1);
					setWalls(bot.curTile + COLS, 0, 0, 0, 1);
					break;
			}
			field[bot.curTile].visited = 1;
			field[bot.curTile + 1].visited = 1;
			field[bot.curTile + COLS].visited = 1;
			field[bot.curTile + COLS + 1].visited = 1;
		}
		else {
			int exitColor;
			if (color == Red) exitColor = Green;
			else exitColor = Red;

			printf("at exit checkpint\n");
			bot.stop();
			printf("estimated tile %d\n", coordToTile(bot.getPos().x, bot.getPos().y));

			int checkpoint = coordToTile(bot.getPos().x, bot.getPos().y);

			// search for red/green tile
			if (bot.getLidarPoint(3, 481) > 9 && bot.getLidarPoint(3, 30) > 9) {
				printf("try north\n");
				bot.move(3);
				// if not checkpoint, backup and try another dir
				if (bot.getTileColor(0, 0) == exitColor) {
					proceed = true;
				}
				else {
					bot.move(-3);
				}
			}
			/*else {
				switch (bot.getDirection()) {
					case North:
						setWalls(bot.curTile, 1, 0, 0, 0);
						setWalls(bot.curTile + 1, 1, 0, 0, 0);
						break;
					case East:
						setWalls(bot.curTile + 1 + COLS, 0, 1, 0, 0);
						setWalls(bot.curTile + 1, 0, 1, 0, 0);
						break;
					case South:
						setWalls(bot.curTile + COLS, 0, 0, 1, 0);
						setWalls(bot.curTile + 1 + COLS, 0, 0, 1, 0);
						break;
					case West:
						setWalls(bot.curTile, 0, 0, 0, 1);
						setWalls(bot.curTile + COLS, 0, 0, 0, 1);
						break;
				}
			}*/

			// try right side
			if (bot.getLidarPoint(3, 157) > 9 && bot.getLidarPoint(3, 225) > 9 && !proceed) {
				printf("try right\n");
				bot.turn(bot.getDirection() + 1); // turn
				bot.move(3);
				if (bot.getTileColor(0, 0) == exitColor) {
					proceed = true;
				}
				else {
					bot.move(-3);
					bot.turn(bot.getDirection() - 1);
				}
			}
			/*else {
				switch (bot.getDirection()) {
					case North:
						setWalls(bot.curTile + 1 + COLS, 0, 1, 0, 0);
						setWalls(bot.curTile + 1, 0, 1, 0, 0);
						break;
					case East:
						setWalls(bot.curTile + COLS, 0, 0, 1, 0);
						setWalls(bot.curTile + 1 + COLS, 0, 0, 1, 0);
						break;
					case South:
						setWalls(bot.curTile, 0, 0, 0, 1);
						setWalls(bot.curTile + COLS, 0, 0, 0, 1);
						break;
					case West:
						setWalls(bot.curTile, 1, 0, 0, 0);
						setWalls(bot.curTile + 1, 1, 0, 0, 0);
						break;
				}
			}*/

			// try left side
			if (bot.getLidarPoint(3, 353) > 9 && bot.getLidarPoint(3, 413) > 9 && !proceed) {
				printf("try left\n");
				bot.turn(bot.getDirection() - 1); // turn
				bot.move(3);
				if (bot.getTileColor(0, 0) == exitColor) {
					proceed = true;
				}
				else {
					bot.move(-3);
					bot.turn(bot.getDirection() + 1);
				}
			}
			/*else {
				switch (bot.getDirection()) {
					case North:
						setWalls(bot.curTile, 0, 0, 0, 1);
						setWalls(bot.curTile + COLS, 0, 0, 0, 1);

						break;
					case East:
						setWalls(bot.curTile, 1, 0, 0, 0);
						setWalls(bot.curTile + 1, 1, 0, 0, 0);

						break;
					case South:
						setWalls(bot.curTile + 1 + COLS, 0, 1, 0, 0);
						setWalls(bot.curTile + 1, 0, 1, 0, 0);


						break;
					case West:
						setWalls(bot.curTile + COLS, 0, 0, 1, 0);
						setWalls(bot.curTile + 1 + COLS, 0, 0, 1, 0);
						break;
				}
			}*/

			if (proceed) {
				printf("direction = %d\n", bot.getDirection());
				bot.stop();
				bot.delay(10000);
			}

		}
		
		break;
	}
}
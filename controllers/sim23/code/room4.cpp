#include "room4.h"

void LOP() {
	char message[1] = { 'L' }; // message = 'L' to activate lack of progress
	bot.emitter->send(message, sizeof(message)); // Send out the message array. Note that the 2nd parameter must be the size of the message
	bot.robot->step(bot.robot->getBasicTimeStep());
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
	bool lidarYes = true;

	printf("start dir %d\n", startDir);
	printf("color %d\n", color);
	//bot.delay(1000);

	getTile(bot.curTile);

	// there should be walls surroundng the green/red tile.
	// look for the checkpoint tile
	// try front
	for (int i = 481; i < 512; i++) {
		if (bot.getLidarPoint(3, i) < 9) lidarYes = false;
	}
	for (int i = 0; i < 30; i++) {
		if (bot.getLidarPoint(3, i) < 9) lidarYes = false;
	}
	if (lidarYes) {
		printf("try front\n");
		bot.move(6);
		// if not checkpoint, backup and try another dir
		if (bot.getTileColor(0, 0) == Checkpoint) {
			proceed = true;
		}
		else {
			printf("front doesn't work\n");
			bot.move(-6);
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
	}

	lidarYes = true;
	for (int i = 97; i < 157; i++) {
		if (bot.getLidarPoint(3, i) < 9) lidarYes = false;
	}

	// try right side
	if (lidarYes && !proceed) {
		printf("try right\n");
		bot.turn(bot.getDirection() + 1); // turn
		bot.move(6);
		if (bot.getTileColor(0, 0) == Checkpoint) {
			proceed = true;
		}
		else {
			printf("right didnt work\n");
			bot.move(-6);
			switch (startDir) {
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
			bot.turn(startDir); // go back to initial position
		}
	}

	lidarYes = true;
	for (int i = 353; i < 413; i++) {
		if (bot.getLidarPoint(3, i) < 9) lidarYes = false;
	}

	// try left side
	if (lidarYes && !proceed) {
		printf("try left\n");
		bot.turn(bot.getDirection() - 1); // turn
		bot.move(6);
		if (bot.getTileColor(0, 0) == Checkpoint) {
			proceed = true;
		}
		else {
			printf("left didnt work\n");
			bot.move(-6);
			switch (startDir) {
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
			bot.turn(startDir);
		}
	}

	// valid room 4, proceed with navigation
	if (proceed) {
		bot.move(9);
		printf("on checkpoint\n");
		checkpointX = bot.getPos().x;
		checkpointY = bot.getPos().y;
		directionEntered = bot.getDirection();

		// DHRUVA PUT YOUR SLAM HERE!		
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
			startDir = bot.getDirection();
			proceed = false;
			lidarYes = true;
			if (color == Red) exitColor = Green;
			else exitColor = Red;

			printf("at exit checkpint\n");
			bot.stop();
			bot.delay(50);
			bot.updatePrevPos();
			bot.turn(directionEntered);
			printf("estimated tile %d\n", coordToTile(bot.getPos().x, bot.getPos().y));

			int checkpoint = coordToTile(bot.getPos().x, bot.getPos().y);

			for (int i = 481; i < 512; i++) {
				if (bot.getLidarPoint(3, i) < 9) lidarYes = false;
			}
			for (int i = 0; i < 30; i++) {
				if (bot.getLidarPoint(3, i) < 9) lidarYes = false;
			}

			// search for red/green tile
			if (lidarYes) {
				printf("try front\n");

				bot.move(3);
				bot.stop();
				bot.delay(1000);
				// if not exit, backup and try another dir
				if (bot.getTileColor(0, 0) == exitColor) {
					proceed = true;
				}
				else {
					printf("not front\n");
					bot.move(-3);
					bot.stop();
					bot.delay(50);
				}
			}

			lidarYes = true;
			for (int i = 97; i < 157; i++) {
				if (bot.getLidarPoint(3, i) < 9) lidarYes = false;
			}
			// try right side
			if (lidarYes && !proceed) {
				printf("try right\n");
				bot.turn(bot.getDirection() + 1); // turn
				bot.move(3);
				bot.stop();
				bot.delay(50);
				if (bot.getTileColor(0, 0) == exitColor) {
					proceed = true;
				}
				else {
					printf("not right\n");
					bot.move(-3);
					bot.stop();
					bot.delay(50);
					bot.turn(startDir);
				}
			}

			lidarYes = true;
			for (int i = 353; i < 413; i++) {
				if (bot.getLidarPoint(3, i) < 9) lidarYes = false;
			}
			// try left side
			if (lidarYes && !proceed) {
				printf("try left\n");
				bot.turn(bot.getDirection() - 1); // turn
				bot.move(3);
				bot.stop();
				bot.delay(50);
				if (bot.getTileColor(0, 0) == exitColor) {
					proceed = true;
				}
				else {
					printf("not left\n");
					bot.move(-3); 
					bot.stop();
					bot.delay(50);
					bot.turn(startDir);
				}
			}

			lidarYes = true;
			for (int i = 225; i < 285; i++) {
				if (bot.getLidarPoint(3, i) < 9) lidarYes = false;
			}
			// try left side
			if (lidarYes && !proceed) {
				printf("try back\n");
				bot.turn(bot.getDirection() + 2); // turn
				bot.move(3);
				if (bot.getTileColor(0, 0) == exitColor) {
					proceed = true;
				}
				else {
					bot.move(-3);
					bot.turn(startDir);
					printf("not back\n");
					// if this fails something rly bad happened
				}
			}

			if (proceed) {
				bot.move(9);
				printf("direction = %d\n", bot.getDirection());
				bot.stop();
				bot.curTile = checkpoint;
				
				/*field[bot.curTile].visited = 1;
				field[bot.curTile + 1].visited = 1;
				field[bot.curTile + COLS].visited = 1;
				field[bot.curTile + COLS + 1].visited = 1;*/
				if (bot.getTileColor(0, 0) == Red) {
					bot.redTile = bot.curTile;
					field[bot.curTile].color = Red;
					bot.curRoom = 3;
				}
				else if (bot.getTileColor(0, 0) == Green) {
					bot.greenTile = bot.curTile;
					field[bot.curTile].color = Green;
					bot.curRoom = 1;
				}
				bot.LOProom4 = true;
				bot.room4done = true;
				
				editMapTile(bot.curTile);
				bot.updatePrevPos();
			}
		}
		
		break;
	}
}
#include "room4.h"

void doRoom4() {
	double x = bot.getPos().x;
	double y = bot.getPos().y;
	int startDir = bot.getDirection();
	int color = bot.getTileColor(0, 0);
	int wallTraceColor;
	int directionEntered = 0;
	bool LOProom4 = false;
	bool proceed = false;

	printf("start dir %d\n", startDir);
	printf("color %d\n", color);
	//bot.delay(1000);

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
		printf("on checkpoint\n");
		bot.move(9);
		directionEntered = bot.getDirection();
		// bot is on checkpoint tile, set as checkpoint
		
		// trace
		while (bot.update() && !LOProom4) {
			if (bot.checkLOPemitter()) {
				bot.curTile = bot.checkpointTile;
				bot.curRoom = bot.checkpointRoom;
				LOProom4 = true;
				break;
			}
			wallTraceColor = room4_wallTrace();
			printf("color = %d\n", wallTraceColor);
			if (color == wallTraceColor) break;
			if (color == Green && wallTraceColor == Red) break;
			if (color == Red && wallTraceColor == Green) break;
		}

		// not at entrance tile
		if (wallTraceColor != color && !LOProom4) {
			printf("turn around!\n");
			switch (bot.roundAngle()) {
				case 0: case 1: bot.turn(South); break;
				case 2: case 3: bot.turn(West); break;
				case 4: case 5: bot.turn(North); break;
				case 6: case 7: bot.turn(East); break;
			}
		

			while (bot.update() && color != room4_wallTrace()) {
				if (bot.checkLOPemitter()) {
					bot.curTile = bot.checkpointTile;
					bot.curRoom = bot.checkpointRoom;
					LOProom4 = true;
					break;
				}
			}
		}

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
		
		field[bot.curTile].visited = 1;
		field[bot.curTile + 1].visited = 1;
		field[bot.curTile + COLS].visited = 1;
		field[bot.curTile + COLS + 1].visited = 1;
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
	//while (bot.update() && color == room4_wallTrace()) {
	//	if (bot.checkLOPemitter()) {
	//		bot.curTile = bot.checkpointTile;
	//		bot.curRoom = bot.checkpointRoom;
	//		LOProom4 = true;
	//		break;
	//	}
	//}
}
#include "lib.h"
#include "bot.h"
#include "bfs.h"
#include "cumulative_map.h"
#include "wallTrace.h"

extern int parent[fieldSize];

int main() {
	int target = -1;

	parent[bot.startTile] = bot.startTile; // set starting point
	for (int i = 0; i < 3 * ROWS + ROWS + 1; i++) {
		for (int j = 0; j < 3 * ROWS + ROWS + 1; j++) {
			bigmap[i][j] = "0";
		}
	}
	field[bot.startTile].color = Start;

	// SIMULATION LOOP
	while (bot.update()) {
		if (bot.getGameTime() < 20) {
			mapBonus();
			break;
		}

		// testing stuff
		/*bot.curRoom = 3;
		getTile(bot.curTile);
		field[bot.curTile].visited = 0;*/

		if (bot.checkLOPemitter()) {
			bot.curTile = bot.checkpointTile;
			bot.curRoom = bot.checkpointRoom;
			printf("lack of progress called\n");
		}

		// Room 4
		if (bot.curRoom == 4 && !bot.room4done) {
			double x = bot.getPos().x;
			double y = bot.getPos().y;
			int startDir = bot.getDirection();
			int color = bot.getTileColor(0, 0);
			int wallTraceColor;
			bool LOProom4 = false;

			printf("start dir %d\n", startDir);
			printf("color %d\n", color);
			//bot.delay(1000);
			
			// get off tile
			while (bot.update() && color == room4_wallTrace()) {
				if (bot.checkLOPemitter()) {
					bot.curTile = bot.checkpointTile;
					bot.curRoom = bot.checkpointRoom;
					LOProom4 = true;
					break;
				}
			}

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

			/*switch (bot.roundAngle()) {
				case 0: case 1:
					bot.turn(North);
					break;
				case 2: case 3:
					bot.turn(East);
					break;
				case 4: case 5:
					bot.turn(South);
					break;
				case 6: case 7:
					bot.turn(West);
					break;
			}
			while (bot.update() && bot.getTileColor(0, 0) == color) {
				bot.speed(5.0, 5.0);
			}*/

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

		getTile(bot.curTile);
		target = bfs(bot.curTile); // do bfs
		printf("cur = %d target = %d\n", bot.curTile, target);
		
		if (target != -1 && bot.curTile != target) {
			bot.curTile = move2Tile(bot.curTile, target);
		}
		else {
			printf("current room = %d\n", bot.curRoom);
			gohome(bot.curTile);
			switch (bot.curRoom) {
				case 4:
				case 3:
					printf("i need to go home! room 3 = %d\n", bot.purpleTile);
					bot.curTile = move2Tile(bot.curTile, bot.purpleTile);
					bot.curRoom = 2;
					//bfs(bot.curTile);
					gohome(bot.curTile);
				case 2:
					printf("i need to go home! room 2 = %d\n", bot.blueTile);
					bot.curTile = move2Tile(bot.curTile, bot.blueTile);
					bot.curRoom = 1;
					//bfs(bot.curTile);
					gohome(bot.curTile);
				case 1:
					printf("i need to go home! start tile = %d cur tile = %d\n", bot.startTile, bot.curTile);
					move2Tile(bot.curTile, bot.startTile);
					mapBonus();
			}
			break;
		}	
	}
	
	return 0;
}

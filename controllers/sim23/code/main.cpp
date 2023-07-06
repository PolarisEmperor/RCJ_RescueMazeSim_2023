#include "lib.h"
#include "bot.h"
#include "bfs.h"
#include "cumulative_map.h"
#include "wallTrace.h"
#include "room4.h"

extern int parent[fieldSize];

// lack of progress called during bfs stuff
void emergency() {
	mapBonus();

	bot.curRoom = bot.checkpointRoom;
	bot.curTile = coordToTile(bot.getPos().x, bot.getPos().y);
	printf("EMERGENCY at %d\n", bot.curTile);
	bot.stop();
	bot.delay(4000);

	bfs(bot.curTile);
	switch (bot.curRoom) {
		case 4:
		case 3:
			printf("i need to go home! room 3 = %d\n", bot.purpleTile);
			bot.curTile = move2Tile(bot.curTile, bot.purpleTile);
			bot.curRoom = 2;
			bfs(bot.curTile);
			//gohome(bot.curTile);
		case 2:
			printf("i need to go home! room 2 = %d\n", bot.blueTile);
			bot.curTile = move2Tile(bot.curTile, bot.blueTile);
			bot.curRoom = 1;
			bfs(bot.curTile);
			gohome(bot.curTile);
		case 1:
			printf("i need to go home! start tile = %d cur tile = %d\n", bot.startTile, bot.curTile);
			move2Tile(bot.curTile, bot.startTile);
	}
	if (!compareCoords(bot.startPos.x, bot.startPos.y, bot.getPos().x, bot.getPos().y)) {
		printf("bfs messed up real bad\n");
		// drive back and forth like a drunk man if not given up on life
		while (bot.update()) {
			bot.speed(6, 6);
			bot.delay(50);
			bot.stop();
			bot.delay(1000);
			bot.speed(-6, -6);
			bot.delay(50);
			bot.stop();
			bot.delay(1000);
		}
	}
	else {
		printf("done\n");
		// Send the letter 'E' to signify exit
		char message = 'E';
		bot.emitter->send(&message, 1);
		while (bot.robot->step(bot.robot->getBasicTimeStep()) != -1);
		exit(0);
	}
}

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
			// Send the letter 'E' to signify exit
			char message = 'E';
			bot.emitter->send(&message, 1);

			while (bot.robot->step(bot.robot->getBasicTimeStep()) != -1);
			break;
		}
		// testing stuff
		/*bot.curRoom = 3;
		getTile(bot.curTile);
		field[bot.curTile].visited = 0;*/
		//printf("%f %f\n", bot.tileToCoords(4552).x, bot.tileToCoords(4552).y);


		if (bot.checkLOPemitter()) {
			bot.curTile = bot.checkpointTile;
			bot.curRoom = bot.checkpointRoom;
			printf("lack of progress called\n");
		}

		// Room 4
		if (bot.curRoom == 4 && !bot.room4done) {
			// do room 4 yay
			doRoom4();
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
					if (bot.checkLOPemitter()) emergency();
					printf("i need to go home! room 3 = %d\n", bot.purpleTile);
					bot.curTile = move2Tile(bot.curTile, bot.purpleTile);
					bot.curRoom = 2;
					//bfs(bot.curTile);
					gohome(bot.curTile);
					if (bot.checkLOPemitter()) emergency();
				case 2:
					printf("i need to go home! room 2 = %d\n", bot.blueTile);
					bot.curTile = move2Tile(bot.curTile, bot.blueTile);
					bot.curRoom = 1;
					//bfs(bot.curTile);
					gohome(bot.curTile);
					if (bot.checkLOPemitter()) emergency();
				case 1:
					printf("i need to go home! start tile = %d cur tile = %d\n", bot.startTile, bot.curTile);
					move2Tile(bot.curTile, bot.startTile);
					if (bot.checkLOPemitter()) emergency();
			}
			break;
		}	
	}
	if (!compareCoords(bot.startPos.x, bot.startPos.y, bot.getPos().x, bot.getPos().y)) {
		char message[1] = { 'L' }; // message = 'L' to activate lack of progress
		bot.emitter->send(message, sizeof(message)); // Send out the message array. Note that the 2nd parameter must be the size of the message
		bot.robot->step(bot.robot->getBasicTimeStep());
		emergency();
	}
	mapBonus();
	printf("done\n");
	// Send the letter 'E' to signify exit
	char message = 'E';
	bot.emitter->send(&message, 1);
	while (bot.robot->step(bot.robot->getBasicTimeStep()) != -1) {
		printf("in here\n");
	}
	return 0;
}

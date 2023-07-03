#include "lib.h"
#include "bot.h"
#include "bfs.h"
#include "cumulative_map.h"
#include "wallTrace.h"
#include "room4.h"

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
			}
			break;
		}	
	}
	mapBonus();
	printf("done\n");
	// Send the letter 'E' to signify exit
	char message = 'E';
	bot.emitter->send(&message, 1);
	while (bot.robot->step(bot.robot->getBasicTimeStep()) != -1);
	return 0;
}

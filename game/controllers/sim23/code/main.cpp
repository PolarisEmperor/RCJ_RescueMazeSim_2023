#include "lib.h"
#include "bot.h"
#include "bfs.h"

extern int parent[fieldSize];

int main(int argc, char **argv) {
	Bot bot;

	int start = fieldSize / 2;
	int cur = start;
	int target = -1;
	
	parent[cur] = cur; // set starting point

	// SIMULATION LOOP
	while (bot.update()) {
		getTile(&bot, cur);
		//printf("doing bfs\n");
		target = bfs(&bot, cur); // do bfs
		printf("cur = %d target = %d\n", cur, target);
		if (target != -1 && cur != target) {
			cur = move2Tile(&bot, cur, target);
		}
		else {
			printf("i need to go home! start tile = %d\n", start);
			move2Tile(&bot, cur, start);
			break;
		}

		
		/*bot.move(6);
		bot.move(6);
		bot.move(6);
		bot.move(6);
		printf("%f %f %f\n", bot.getLidar(3, 0), bot.getLidar(3, 496), bot.getLidar(3, 15));
		break;*/
		//bot.turn(90);
		
		//printf("%f\n", bot.getLidar(3, 0));

		//getTile(&bot, cur);
		
	}
	bot.destroy();
	return 0;
}

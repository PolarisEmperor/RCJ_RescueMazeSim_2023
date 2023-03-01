#include "lib.h"
#include "bot.h"
#include "bfs.h"

extern int parent[fieldSize];

int main(int argc, char **argv) {
	int target = -1;

	parent[bot.tile.room1] = bot.tile.room1; // set starting point

	// SIMULATION LOOP
	while (bot.update()) {
		//bot.curRoom = 2;
		getTile(bot.tile.cur);
		target = bfs(bot.tile.cur); // do bfs
		printf("cur = %d target = %d\n", bot.tile.cur, target);
		//bot.delay(10000000);
		//break;
		
		
		
		if (target != -1 && bot.tile.cur != target) {
			bot.tile.cur = move2Tile(bot.tile.cur, target);
		}
		else {
			printf("i need to go home! start tile = %d\n", bot.tile.room1);
			switch (bot.curRoom) {
				case 2:
					bot.tile.cur = move2Tile(bot.tile.cur, bot.tile.room2);
					bot.curRoom = 1;
				case 1:
					move2Tile(bot.tile.cur, bot.tile.room1);
			}
			break;
		}
		
	}

	bot.destroy();
	return 0;
}

#include "lib.h"
#include "bot.h"
#include "bfs.h"

extern int parent[fieldSize];

int main(int argc, char **argv) {
	int target = -1;

	parent[bot.tile.room1] = bot.tile.room1; // set starting point

	// SIMULATION LOOP
	while (bot.update()) {
		getTile(bot.tile.cur);
		target = bfs(bot.tile.cur); // do bfs
		printf("cur = %d target = %d\n", bot.tile.cur, target);
		
		if (target != -1 && bot.tile.cur != target) {
			bot.tile.cur = move2Tile(bot.tile.cur, target);
		}
		else {
			switch (bot.curRoom) {
				case 2:
					printf("i need to go home! start tile = %d\n", bot.tile.room2);
					bot.tile.cur = move2Tile(bot.tile.cur, bot.tile.room2);
					bot.curRoom = 1;
					bfs(bot.tile.cur);
					bot.delay(3000);
				case 1:
					printf("i need to go home! start tile = %d\n", bot.tile.room1);
					move2Tile(bot.tile.cur, bot.tile.room1);
			}
			break;
		}

	}

	bot.destroy();
	return 0;
}

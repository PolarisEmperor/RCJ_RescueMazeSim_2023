#include "lib.h"
#include "bot.h"
#include "bfs.h"

extern int parent[fieldSize];

int main() {
	int target = -1;

	parent[bot.startTile] = bot.startTile; // set starting point
	memset(bigmap, '0', sizeof(bigmap));

	
	// SIMULATION LOOP
	while (bot.update()) {
		getTile(bot.curTile);
		target = bfs(bot.curTile); // do bfs
		//printf("cur = %d target = %d\n", bot.curTile, target);
		
		if (target != -1 && bot.curTile != target) {
			bot.curTile = move2Tile(bot.curTile, target);
		}
		else {
			printf("current room = %d\n", bot.curRoom);
			switch (bot.curRoom) {
				case 4:
				case 3:
					printf("i need to go home! room 3 = %d\n", bot.purpleTile);
					bot.curTile = move2Tile(bot.curTile, bot.purpleTile);
					bot.curRoom = 2;
					bfs(bot.curTile);
					//bot.delay(3000);
				case 2:
					printf("i need to go home! room 2 = %d\n", bot.blueTile);
					bot.curTile = move2Tile(bot.curTile, bot.blueTile);
					bot.curRoom = 1;
					bfs(bot.curTile);
					//bot.delay(3000);
				case 1:
					printf("i need to go home! start tile = %d\n", bot.startTile);
					move2Tile(bot.curTile, bot.startTile);
			}
			break;
		}
	}
	
	return 0;
}

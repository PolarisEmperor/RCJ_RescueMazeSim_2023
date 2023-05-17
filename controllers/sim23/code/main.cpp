#include "lib.h"
#include "bot.h"
#include "bfs.h"
#include "cumulative_map.h"

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
				case 2:
					printf("i need to go home! room 2 = %d\n", bot.blueTile);
					bot.curTile = move2Tile(bot.curTile, bot.blueTile);
					bot.curRoom = 1;
					bfs(bot.curTile);
				case 1:
					printf("i need to go home! start tile = %d\n", bot.startTile);
					move2Tile(bot.curTile, bot.startTile);
					mapBonus();
			}
			break;
		}
	}
	
	return 0;
}

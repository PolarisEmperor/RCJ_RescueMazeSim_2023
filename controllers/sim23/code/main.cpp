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
	}

	bot.destroy();
	return 0;
}

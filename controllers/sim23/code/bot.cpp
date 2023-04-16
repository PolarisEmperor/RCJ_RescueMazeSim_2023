#include "bot.h"
#include "tile.h"
#include "victim.h"

#define maxSpd (lm->getMaxVelocity() - 0.01)
#define timeStep ((int)robot->getBasicTimeStep())

using namespace std;
using namespace webots;

Bot bot;

// Robot constructor
Bot::Bot() {
	// Enable motors / sensors
	gps->enable(timeStep);
	camL->enable(timeStep);
	camR->enable(timeStep);
	camB->enable(timeStep);
	lidar->enable(timeStep);
	receiver->enable(timeStep);
	encL->enable(timeStep);
	encR->enable(timeStep);
	imu->enable(timeStep);
	lm->setPosition(INFINITY);
	rm->setPosition(INFINITY);
	lm->setVelocity(0);
	rm->setVelocity(0);

	// Set room values
	curRoom = 1;
	curTile = startTile = fieldSize / 2;
	checkpointTile = curTile;
	blueTile = purpleTile =	redTile = greenTile = -1;

	startPos = { -9999, -9999 };
}

// Robot destructor
Bot::~Bot() {
	sendMap();

	// Send the letter 'E' to signify exit
	char message = 'E';
	emitter->send(&message, 1);

	delete robot;
}

// Delay function
void Bot::delay(int ms) {
	double initTime = robot->getTime();
	while (update()) {
		if ((robot->getTime() - initTime) * 1000.0 > ms) {
			break;
		}
	}
}

// Update simulation loop
bool Bot::update() {
	// Timestep
	if (robot->step(timeStep) == -1) return 0;

	// Update angle
	angle = getAngle();

	// Update GPS position
	pos.x = (double)gps->getValues()[0];
	pos.y = (double)gps->getValues()[2];

	if (startPos.x == -9999) startPos = pos;
	if (prevPos.x == 0) updatePrevPos();

	// Update direction
	curDir = getDirection();
	return 1;
}

// Get current position
Bot::Pos Bot::getPos() {
	return pos;
}

// Get previous position
Bot::Pos Bot::getPrevPos() {
	return prevPos;
}

// Update previous position variable
void Bot::updatePrevPos() {
	prevPos.x = pos.x;
	prevPos.y = pos.y;
}

// Testing something (not working)
Bot::Pos Bot::getTilePos(int tile) {
	Pos tilePos;
	int row = tile / ROWS;
	int col = tile % COLS;
	int startRow = startTile / ROWS;
	int startCol = startTile % COLS;
	int diffX = startCol - col;
	int diffY = abs(startRow - row);

	if (diffX < 0) diffX = (COLS + diffX) * -1;

	//if (row < startRow) diffX = (COLS + diffX) * -1;
	//if (col < startCol) diffY = (ROWS - diffY) * -1;

	/*tilePos.x = startPos.x + (0.06 * (col - startCol));
	tilePos.y = startPos.y + (0.06 * (row - startRow));*/
	printf("r %d c  %d sr %d sc %d\n", row, col, startRow, startCol);
	printf("diff %d %d\n", diffX, diffY);
	//printf("%f %f\n", tilePos.x, tilePos.y);

	return pos;
}

// Get lidar readings from specified point and layer in cm
const float Bot::getLidar(int layer, int point) {
	return lidar->getRangeImage()[(lidar->getHorizontalResolution() * layer) + point] * 100;
}

// Get current angle
double Bot::getAngle() {
	return imu->getRollPitchYaw()[2];
}

// Get current global compass direction
int Bot::getDirection() {
	double err = 0.05;
	if (angle > 0 - err && angle < 0 + err)
		return North;
	else if (angle > -1.57 - err && angle < -1.57 + err)
		return East;
	else if ((angle > 3.14 - err && angle < 3.14 + err) ||
			 (angle > -3.14 - err && angle < -3.14 + err))
		return South;
	else if (angle > 1.57 - err && angle < 1.57 + err)
		return West;

	return -1;
}

// Set motor speeds
void Bot::speed(double lSpd, double rSpd) {
	lm->setVelocity(min((float)maxSpd, max((float)lSpd, (float)(maxSpd * -1))));
	rm->setVelocity(min((float)maxSpd, max((float)rSpd, (float)(maxSpd * -1))));
}

// Stop motors
void Bot::stop() {
	speed(0, 0);
	delay(1);
}

// Move # cm
// Forward = +cm Backward = -cm
void Bot::move(double cm, double spd) {
	double target = 0;

	switch (curDir) {
		case North:
			target = prevPos.y - cm / 100;
			break;
		case East:
			target = prevPos.x + cm / 100;
			break;
		case South:
			target = prevPos.y + cm / 100;
			break;
		case West:
			target = prevPos.x - cm / 100;
			break;
	}

	if (cm > 0)
		while (update()) {
			if (curDir == North && pos.y <= target) break;
			if (curDir == South && pos.y >= target) break;
			if (curDir == East && pos.x >= target) break;
			if (curDir == West && pos.x <= target) break;
			speed(spd, spd);
		}
	else if (cm < 0)
		while (update()) {
			if (curDir == North && pos.y >= target) break;
			if (curDir == South && pos.y <= target) break;
			if (curDir == East && pos.x <= target) break;
			if (curDir == West && pos.x >= target) break;
			speed(-spd, -spd);
		}

	stop();
	updatePrevPos();
}

// Turn to global compass direction
// N:0 E:1 S:2 W:3
void Bot::turn(int dir, double spd) {
	double angles[] = { 0, -1.57, 3.14, 1.57 };
	double err = 0.015;

	int prevDir = getDirection();
	while (update()) {
		if (bot.getLidar(3, 127) < 8 && !field[bot.curTile].victimChecked) {
			checkVisualVictim(bot.camR);
			printf("found victim turning\n");
			//field[bot.curTile].victimChecked = 1;
		}
		if (bot.getLidar(3, 383) < 8 && !field[bot.curTile].victimChecked) {
			checkVisualVictim(bot.camL);
			printf("found victim turning\n");
			//field[bot.curTile].victimChecked = 1;
		}

		if (dir == South && fabs(angle) > angles[dir] - err && fabs(angle) < angles[dir] + err)
			break;

		if (angle > angles[dir] - err && angle < angles[dir] + err)
			break;

		if (prevDir < dir)
			(abs(dir - prevDir) == 3) ? speed(-spd, spd) : speed(spd, -spd);
		else if (prevDir > dir)
			(abs(dir - prevDir) == 3) ? speed(spd, -spd) : speed(-spd, spd);
	}
	stop();

	updatePrevPos();
}

// Get tile color
int Bot::getTileColor(int x, int y) {
	const int holeThreshold = 32;
	const int checkptThreshold = 188;
	const int whiteConst = 192;
	const int closeThreshold = 0;

	/*0: tile
	  2: hole
	  3: swamp
	  4: checkpoint
	  6: blue (1-2)
	  7: purple (1-3)
	  8: red (2-3) */

	const unsigned char *img = camB->getImage();
	unsigned char r = Camera::imageGetRed(img, camB->getWidth(), x, y);
	unsigned char g = Camera::imageGetGreen(img, camB->getWidth(), x, y);
	unsigned char b = Camera::imageGetBlue(img, camB->getWidth(), x, y);

	if (r == whiteConst && g == whiteConst && b == whiteConst) return Normal;
	if (r < holeThreshold && g < holeThreshold && b < holeThreshold) return Hole;
	if (r > checkptThreshold && g > checkptThreshold && b > checkptThreshold) return Checkpoint;

	int minColor = min(r, min(g, b));

	bool hi[3] = { r - minColor > closeThreshold, g - minColor > closeThreshold, b - minColor > closeThreshold };

	if (hi[0] && !hi[1] && !hi[2]) return Red;
	if (!hi[0] && !hi[1] && hi[2]) return Blue;
	if (hi[0] && !hi[1] && hi[2]) return Purple;
	if (hi[0] && hi[1] && !hi[2]) return Swamp;
	if (!hi[0] && hi[1] && hi[2]) return Checkpoint;

	return Normal;
}
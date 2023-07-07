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
	curTile = startTile = (ROWS / 2 * COLS) + (COLS / 2);
	checkpointTile = curTile;
	checkpointRoom = curRoom;
	blueTile = purpleTile =	redTile = greenTile = -1;
	room4done = false;
	LOProom4 = false;

	startPos = { -9999, -9999 };
}

// Robot destructor
Bot::~Bot() {
	delete robot;
}

// Delay function
void Bot::delay(int ms) {
	double initTime = robot->getTime();
	while (update()) {
		bot.checkRT();
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

Bot::Pos Bot::getTargetPos(int diffX, int diffY) {
	Pos targetPos = startPos;

	targetPos.x += diffX * 0.06;
	targetPos.y += diffY * 0.06;

	return targetPos;
}

Bot::Pos Bot::tileToCoords(int tile) {
	Pos pos = bot.startPos;
	// get tile to the same row as start tile
	while (tile / 100 != bot.startTile / 100) {
		if (tile / 100 < bot.startTile / 100) {
			pos.y -= 0.06;
			tile += COLS;
		}
		else {
			pos.y += 0.06;
			tile -= COLS;
		}
	}
	// get tile to the same col as the start tile
	while (tile % 100 != bot.startTile % 100) {
		if (tile % 100 < bot.startTile % 100) {
			pos.x -= 0.06;
			tile ++;
		}
		else {
			pos.x += 0.06;
			tile --;
		}
	}
	return pos;
}

// Get lidar readings from specified point and layer in cm
const float Bot::getLidarPoint(int layer, int point) {
	return lidar->getRangeImage()[(lidar->getHorizontalResolution() * layer) + point] * 100;
}

const float* Bot::getLidarLayer(int layer) {
	return lidar->getLayerRangeImage(layer);
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
	//delay(1);
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
			checkRT();
		}
	else if (cm < 0)
		while (update()) {
			if (curDir == North && pos.y >= target) break;
			if (curDir == South && pos.y <= target) break;
			if (curDir == East && pos.x <= target) break;
			if (curDir == West && pos.x >= target) break;
			speed(-spd, -spd);
			checkRT();
		}

	stop();
	updatePrevPos();
}

int Bot::roundAngle() {
	enum Directions { N0, N1, E0, E1, S0, S1, W0, W1 };

	if (angle > 0 && angle < 0.785)
		return N0;
	if (angle > -0.785 && angle < 0)
		return N1;
	if (angle > -1.57 && angle < -0.785)
		return E0;
	if (angle > -2.355 && angle < -1.57)
		return E1;
	if (angle >= -3.15 && angle <= -2.355)
		return S0;
	if (angle >= 2.355 && angle <= 3.15)
		return S1;
	if (angle > 1.57 && angle < 2.355)
		return W0;
	if (angle > 0.785 && angle < 1.57)
		return W1;

	return -1;
}

// Turn to global compass direction
// N:0 E:1 S:2 W:3
void Bot::turn(int dir, double spd) {
	double angles[] = { 0, -1.57, 3.14, 1.57 };
	double err = 0.015;

	if (dir < 0) dir += 4;
	else if (dir > 3) dir -= 4;

	int prevDir = getDirection();
	while (update()) {
		checkRT();
		if (bot.getLidarPoint(3, 127) < 7 && !field[bot.curTile].victimChecked) {
			char victim = checkVisualVictim(bot.camR);
			if (victim > 0) {
				//printf("direction %d\n", bot.getDirection());
				int wall = bot.roundAngle() + 2;
				//printf("%d\n", wall);
				if (wall >= 8) wall -= 8;
				mapAngledVictim(bot.curTile, wall, victim);
			}
		}
		if (bot.getLidarPoint(3, 383) < 7 && !field[bot.curTile].victimChecked) {
			char victim = checkVisualVictim(bot.camL);
			if (victim > 0) {
				//printf("direction %d\n", bot.getDirection());
				int wall = bot.roundAngle() - 2;
				//printf("%d\n", wall);
				if (wall < 0) wall += 8;
				mapAngledVictim(bot.curTile, wall, victim);
			}
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
	//stop();

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

	//printf("R: %d, G: %d, B: %d\n", r, g, b);

	if (r == whiteConst && g == whiteConst && b == whiteConst) return Normal;
	if (r < holeThreshold && g < holeThreshold && b < holeThreshold) return Hole;
	if (r > checkptThreshold && g > checkptThreshold && b > checkptThreshold) return Checkpoint;

	int minColor = min(r, min(g, b));

	bool hi[3] = { r - minColor > closeThreshold, g - minColor > closeThreshold, b - minColor > closeThreshold };

	if (hi[0] && !hi[1] && !hi[2]) {
		//printf("RED "); 
		return Red;
	}
	if (!hi[0] && !hi[1] && hi[2]) {
		//printf("BLUE "); 
		return Blue;
	}
	if (hi[0] && !hi[1] && hi[2]) {
		//printf("PURPLE "); 
		return Purple;
	}
	if (hi[0] && hi[1] && !hi[2]) {
		//printf("SWAMP "); 
		return Swamp;
	}
	if (!hi[0] && hi[1] && hi[2]) {
		//printf("CHECKPT "); 
		return Checkpoint;
	}
	if (!hi[0] && hi[1] && !hi[2]) {
		//printf("GREEN "); 
		return Green;
	}

	return Normal;
}

bool Bot::checkLOPemitter() {
	if (receiver->getQueueLength() > 0) {
		char* message = (char*)receiver->getData();
		printf("%s\n", message);
		if (message[0] == 'L') {
			printf("lop\n");
			receiver->nextPacket();
			return true;
		}
		receiver->nextPacket();
	}
	return false;
}

int Bot::getGameTime() {
	float score = 0.0;
	int time = 9999;
	char message[1] = { 'G' }; // message = 'G' for game information
	emitter->send(message, sizeof(message)); // Send out the message array. Note that the 2nd parameter must be the size of the message

	if (receiver->getQueueLength() > 0) { // If receiver queue is not empty
		char *receivedData = (char *)receiver->getData(); // Grab data as a string
		
		if (receivedData[0] == 'G') {
			memcpy(&score, receivedData + 4, 4); // Score stored in bytes 4 to 7
			memcpy(&time, receivedData + 8, 4);  // Remaining time stored in bytes 8 to 11
			//cout << "Game Score: " << score << " Remaining time: " << time << endl;
			receiver->nextPacket(); // Discard the current data packet
		}
	}
	return time;
}

void Bot::checkRT() {
	int dummy = seconds + 1;
	seconds = difftime(time(0), startingtime);
	if (seconds == dummy)
	{
		dummy = seconds + 1;
		printf("real seconds: %d \n", seconds);
		printf("real seconds: %d \n", seconds);
		printf("real seconds: %d \n", seconds);
		printf("real seconds: %d \n", seconds);
		printf("real seconds: %d \n", seconds);
		printf("real seconds: %d \n", seconds);
		printf("real seconds: %d \n", seconds);
		printf("%d \n", seconds >= (realseconds - buffertime));
	}
	if (seconds >= (realseconds - buffertime))
	{
		mapBonus();
		printf("done\n");
		char message = 'E';
		bot.emitter->send(&message, 1);
		exit(0);
	}
}
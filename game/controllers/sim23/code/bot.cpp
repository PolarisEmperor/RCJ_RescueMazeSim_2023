#include "bot.h"

// Robot constructor
Bot::Bot() {
	//gyro->enable(timeStep);
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

	// Update direction
	curDir = getDirection();

	if (prevPos == -1)
		if (curDir == North || curDir == South) prevPos = pos.x;
		else if (curDir == East || curDir == West) prevPos = pos.y;

	return 1;
}

void Bot::destroy() {
	char message = 'E';
	emitter->send(&message, 1); // Send the letter 'E' to signify exit
	delete robot;
}

// Get lidar readings from specified point and layer in cm
const float Bot::getLidar(int layer, int point) {
	return lidar->getRangeImage()[(lidar->getHorizontalResolution() * layer) + point] * 100;
}

int Bot::getLidarRes() {
	return lidar->getHorizontalResolution();
}

// Get current position
Bot::Pos Bot::getPos() {
	return pos;
}

// Get current angle
double Bot::getAngle() {
	return imu->getRollPitchYaw()[2];
}

int Bot::getDirection() {
	double err = 0.03;
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

// Reset left and right encoders
void Bot::resetEnc() {
	resetLeftEnc = encL->getValue() * 180 / PI;
	resetRightEnc = encR->getValue() * 180 / PI;
}

// Get left encoder value
double Bot::getEncL() {
	return encL->getValue() * 180 / PI - resetLeftEnc;
}

// Get right encoder value
double Bot::getEncR() {
	return encR->getValue() * 180 / PI - resetRightEnc;
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
int Bot::move(double cm, double spd) {
	double target = (curDir <= East) ? prevPos + cm / 100 : prevPos - cm / 100;
	bool hole = false;
	int tileColor = 0;
	double distFromWall = 5.5;

	if (cm > 0) {
		while (update()) {
			tileColor = getColor(camB->getWidth() / 2, 25);
			if (tileColor == 2) { // bottom cam sees black
				hole = true;
				break;
			}
			else if (tileColor == 3) {
				spd = 2;
			}
			if (curDir == North && pos.x >= target) break;
			if (curDir == South && pos.x <= target) break;
			if (curDir == East && pos.y >= target) break;
			if (curDir == West && pos.y <= target) break;
			if (getLidar(3, 0) < distFromWall) break;
			speed(spd, spd);
		}
		stop();
	}


	resetEnc();
	if (cm > 0) {
		while (update() && getEncL() < cm * encpercm) {
			tileColor = getColor(camB->getWidth() / 2, 25);
			if (tileColor == 2) { // bottom cam sees black
				hole = true;
				break;
			}
			else if (tileColor == 3) {
				spd = 2;
			}
			speed(spd, spd);
		}

	if (hole) {
		// back up to prev pos
		speed(-spd, -spd);
		printf("theres a black hole, backing up\n");
			
		while (update()) {
			if (getDirection() == North || getDirection() == South) {
				if (fabs(getPos().x - prevPos) < 0.000001 || getLidar(3, 255) < 4.5) break;
			}
			else {
				if (fabs(getPos().y - prevPos) < 0.000001 || getLidar(3, 255) < 4.5) break;
			}
		}
		stop(); 
			
		return 0;
	}

	if (getColor(camB->getWidth() / 2, 25) == 3) printf("SWAMP\n");
	if (getColor(camB->getWidth() / 2, 25) == 4) printf("CHECKPOINT\n");
	if (getColor(camB->getWidth() / 2, 25) == 6) printf("BLUE\n");
	if (getColor(camB->getWidth() / 2, 25) == 7) printf("PURPLE\n");
	if (getColor(camB->getWidth() / 2, 25) == 8) printf("RED\n");

	if (curDir == North || curDir == South) prevPos = pos.x;
	else if (curDir == East || curDir == West) prevPos = pos.y;
	return 1;
}

//double fixAngle(double ang) {
//	bool neg = 0;
//	if (ang < 0) neg = 1;
//
//	if ((ang > 85 && ang < 95) || (ang > -95 && ang < -85)) ang = 90;
//	else if ((ang > 175 && ang < 185) || (ang > -185 && ang < -175)) ang = 180;
//	else if ((ang > 265 && ang < 275) || (ang > -275 && ang < -265)) ang = 270;
//	else if ((ang > 355 && ang < 365)) return 360;
//	else if ((ang > -365 && ang < -355)) return -360;
//	else return -1;
//	return (neg) ? ang * -1 : ang;
//}

// Turn to global compass direction
// N:0 E:1 S:2 W:3
void Bot::turn(int dir, double spd) {
	double angles[] = { 0, -1.57, 3.14, 1.57 };
	double err = 0.015;

	int prevDir = getDirection();
	while (update()) {
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

	if (curDir == North || curDir == South) prevPos = pos.x;
	else if (curDir == East || curDir == West) prevPos = pos.y;
}

// get color from the bottom camera
int Bot::getColor(int x, int y) {
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

	if (r == whiteConst && g == whiteConst && b == whiteConst) return 0;
	if (r < holeThreshold && g < holeThreshold && b < holeThreshold) return 2;
	if (r > checkptThreshold && g > checkptThreshold && b > checkptThreshold) return 4;

	int minColor = min(r, min(g, b));

	bool hi[3] = { r - minColor > closeThreshold, g - minColor > closeThreshold, b - minColor > closeThreshold };


	if (hi[0] && !hi[1] && !hi[2]) return 8;
	if (!hi[0] && !hi[1] && hi[2]) {
		return 6;
	}
	if (hi[0] && !hi[1] && hi[2]) return 7;
	if (hi[0] && hi[1] && !hi[2]) return 3;
	if (!hi[0] && hi[1] && hi[2]) return 4;

	return 0;

}
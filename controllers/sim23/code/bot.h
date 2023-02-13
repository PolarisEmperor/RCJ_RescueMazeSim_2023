#pragma once

#include "lib.h"

enum Direction { North, East, South, West };
enum TileColor { Normal, Hole = 2, Swamp, Checkpoint, Blue = 6, Purple, Red };

class Bot {
private:
	Robot *robot = new Robot();
	Motor *lm = robot->getMotor("wheel2 motor");
	Motor *rm = robot->getMotor("wheel1 motor");
	PositionSensor *encL = robot->getPositionSensor("wheel2 sensor");
	PositionSensor *encR = robot->getPositionSensor("wheel1 sensor");
	GPS *gps = robot->getGPS("gps");
	Camera *camR = robot->getCamera("rCam");
	Camera *camL = robot->getCamera("lCam");
	Camera *camB = robot->getCamera("bCam");
	Lidar *lidar = robot->getLidar("lidar");
	Emitter *emitter = robot->getEmitter("emitter");
	Receiver *receiver = robot->getReceiver("receiver");
	InertialUnit *imu = robot->getInertialUnit("imu");

	const double PI = 3.141592;
	const double maxSpd = lm->getMaxVelocity() - 0.01;
	const int timeStep = (int)robot->getBasicTimeStep();

	// Current position
	struct Pos {
		double x = 0;
		double y = 0;
	} pos, prevPos;

	// Angle in radians
	double angle = 0;

	// Current global compass direction
	int curDir = -1;

	void updatePrevPos();
public:
	Bot();
	void delay(int ms);
	bool update();
	void destroy();
	const float getLidar(int layer, int point);
	int getLidarRes();
	Pos getPos();
	double getAngle();
	int getDirection();
	void speed(double lSpd, double rSpd);
	void stop();
	int move(double cm, double spd = 5);
	void turn(int dir, double spd = 2);
	int getTileColor(int x, int y);
};

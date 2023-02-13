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

	// Current position
	struct Pos {
		double x = 0;
		double y = 0;
	} pos;

	// Angle in radians
	double angle = 0;

	const double PI = 3.141592;
	const double maxSpd = lm->getMaxVelocity() - 0.01;
	const int timeStep = (int)robot->getBasicTimeStep();

	// Position of previous tile
	double prevPos = -1;

	// Current global compass direction
	int curDir = -1;

	void updatePrevPos();
public:
	Bot();
	void delay(int);
	bool update();
	void destroy();
	const float getLidar(int layer, int point);
	int getLidarRes();
	Pos getPos();
	double getAngle();
	int getDirection();
	void speed(double, double);
	void stop();
	int move(double, double = 5);
	void turn(int, double = 2);
	int getColor(int, int);
};

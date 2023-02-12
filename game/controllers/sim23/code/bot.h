#pragma once

#include "lib.h"

enum Direction { North, East, South, West };

class Bot {
private:
	Robot *robot = new Robot();
	Motor *lm = robot->getMotor("wheel2 motor");
	Motor *rm = robot->getMotor("wheel1 motor");
	PositionSensor *encL = robot->getPositionSensor("wheel2 sensor");
	PositionSensor *encR = robot->getPositionSensor("wheel1 sensor");
	//Gyro *gyro = robot->getGyro("gyro");
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
	const double WB = 5.67; // Wheel base
	const double WD = 4.04; // (old) 4.02 // Wheel diameter
	const double encpercm = 360 / (WD * PI);
	const double encperdeg = WB / WD;
	const double maxSpd = lm->getMaxVelocity() - 0.01;
	const int timeStep = (int)robot->getBasicTimeStep();
	
	// Motor encoder
	double resetLeftEnc = 0.0f;
	double resetRightEnc = 0.0f;

	double prevPos = -1;
	int curDir = -1;

	void resetEnc();
	double getEncL();
	double getEncR();

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
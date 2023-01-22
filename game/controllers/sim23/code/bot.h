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
	Gyro *gyro = robot->getGyro("gyro");
	GPS *gps = robot->getGPS("gps");
	Camera *camR = robot->getCamera("rCam");
	Camera *camL = robot->getCamera("lCam");
	Camera *camB = robot->getCamera("bCam");
	Lidar *lidar = robot->getLidar("lidar");
	Emitter *emitter = robot->getEmitter("emitter");
	Receiver *receiver = robot->getReceiver("receiver");
	
	// Current position
	struct Pos {
		double x = 0;
		double y = 0;
	} pos;

	// Current angle in degrees
	double angle = 0; 
		
	const float PI = 3.14f;
	const float WB = 5.67f; // Wheel base
	const float WD = 4.04f; // old = 4.02f; // Wheel diameter
	const float encpercm = (float)(360 / (WD * PI));
	const float encperdeg = (float)(WB / WD);

	// Motor encoder
	double resetLeftEnc = 0.0f;
	double resetRightEnc = 0.0f;

	double maxSpd = lm->getMaxVelocity() - 0.01;

	int timeStep = (int)robot->getBasicTimeStep();

	double prevPos = 0;

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
	void speed(float, float);
	void stop();
	int move(float, float = 5);
	void turn(float);
	int getColor(int, int);
};
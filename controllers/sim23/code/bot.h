#pragma once
#include "lib.h"

enum Direction { North, East, South, West };
enum TileColor { Normal, Hole = 2, Swamp, Checkpoint, Blue = 6, Purple, Red, Green };

const int ROWS = 40;
const int COLS = 40;
const int fieldSize = ROWS * COLS;

class Bot {
private:
	Robot *robot = new Robot();
	Motor *lm = robot->getMotor("wheel2 motor");
	Motor *rm = robot->getMotor("wheel1 motor");

	// Current position
	struct Pos {
		double x = 0;
		double y = 0;
	} pos, prevPos;

	const double PI = 3.141592;
	const double maxSpd = lm->getMaxVelocity() - 0.01;
	const int timeStep = (int)robot->getBasicTimeStep();

	// Angle in radians
	double angle = 0;

	// Current global compass direction
	int curDir = -1;

public:
	Emitter* emitter = robot->getEmitter("emitter");
	Receiver *receiver = robot->getReceiver("receiver");
	GPS *gps = robot->getGPS("gps");
	InertialUnit *imu = robot->getInertialUnit("imu");
	PositionSensor *encL = robot->getPositionSensor("wheel2 sensor");
	PositionSensor *encR = robot->getPositionSensor("wheel1 sensor");
	Camera *camR = robot->getCamera("rCam");
	Camera *camL = robot->getCamera("lCam");
	Camera *camB = robot->getCamera("bCam");
	Lidar *lidar = robot->getLidar("lidar");

	int curRoom;	// Current room
	int curTile;	// Current Tile
	int startTile;	// Starting tile
	int blueTile;	// Room 1 -> Room 2
	int purpleTile; // Room 2 -> Room 3
	int redTile;	// Room 3 -> Room 4
	int greenTile;	// Room 1 -> Room 4

	Bot();
	~Bot();
	void delay(int ms);
	bool update();
	const float getLidar(int layer, int point);
	Pos getPos();
	Pos getPrevPos();
	double getAngle();
	int getDirection();
	void speed(double lSpd, double rSpd);
	void stop();
	void move(double cm, double spd = 6);
	void turn(int dir, double spd = 2);
	int getTileColor(int x, int y);
	void updatePrevPos();
};

extern class Bot bot;
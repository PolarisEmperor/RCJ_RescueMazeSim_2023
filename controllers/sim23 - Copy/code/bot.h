#pragma once
#include "lib.h"

enum Direction { North, East, South, West };

const int ROWS = 100;
const int COLS = 100;
const int fieldSize = ROWS * COLS;

class Bot {
private:
	struct Pos {
		double x = 0;
		double y = 0;
	};
	// Current position
	Pos pos, prevPos;

	// Angle in radians
	double angle = 0;

	// Current global compass direction
	int curDir = -1;
public:
	webots::Robot *robot = new webots::Robot();
	webots::Motor *lm = robot->getMotor("wheel2 motor");
	webots::Motor *rm = robot->getMotor("wheel1 motor");
	webots::Emitter* emitter = robot->getEmitter("emitter");
	webots::Receiver *receiver = robot->getReceiver("receiver");
	webots::GPS *gps = robot->getGPS("gps");
	webots::InertialUnit *imu = robot->getInertialUnit("imu");
	webots::PositionSensor *encL = robot->getPositionSensor("wheel2 sensor");
	webots::PositionSensor *encR = robot->getPositionSensor("wheel1 sensor");
	webots::Camera *camR = robot->getCamera("rCam");
	webots::Camera *camL = robot->getCamera("lCam");
	webots::Camera *camB = robot->getCamera("bCam");
	webots::Lidar *lidar = robot->getLidar("lidar");

	Pos startPos;

	int curRoom;		// Current room
	int curTile;		// Current Tile
	int startTile;		// Starting tile
	int checkpointTile; // Checkpoint Tile
	int checkpointRoom; // Checkpoinst Room
	int blueTile;		// Room 1 -> Room 2
	int purpleTile;		// Room 2 -> Room 3
	int redTile;		// Room 3 -> Room 4
	int greenTile;		// Room 1 -> Room 4
	bool room4done;		// has it traversed room 4 yet
	bool LOProom4;
	int seconds;
	int realseconds;
	int buffertime;
	time_t startingtime;

	Bot();
	~Bot();
	void delay(int ms);
	bool update();
	const float getLidarPoint(int layer, int point);
	const float* getLidarLayer(int layer);
	Pos getPos();
	Pos getPrevPos();
	Pos tileToCoords(int tile);
	Pos getTargetPos(int, int);
	double getAngle();
	int getDirection();
	void speed(double lSpd, double rSpd);
	void stop();
	void move(double cm, double spd = 6);
	void turn(int dir, double spd = 2);
	int getTileColor(int x, int y);
	void updatePrevPos();
	bool checkLOPemitter();
	int roundAngle();
	int getGameTime();
	void checkRT();
};

extern class Bot bot;
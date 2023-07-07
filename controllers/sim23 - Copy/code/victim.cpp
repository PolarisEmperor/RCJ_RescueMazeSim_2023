#include "victim.h"

using namespace webots;
using namespace std;
using namespace cv;

//#define DEBUG_MODE

// constants
const int max_value_H = 360 / 2;
const int max_value = 255;
const int low_H = 150, low_S = 60, low_V = 60;
const int high_H = max_value_H, high_S = max_value, high_V = max_value;
const int low_Hy = 0;
const int high_Hy = 40;
const int thresh = 192;
const int max_thresh = 255;

// gps position
int PosX;
int PosZ;

void sendVictimSignal(char ch) {
	char message[9] = { 0 };

	int x = bot.getPos().x * 100;
	int y = bot.getPos().y * 100;

	int victim_pos[2] = { x, y };

	memcpy(message, victim_pos, sizeof(victim_pos));
	message[8] = ch;

	if (ch == 'H' || ch == 'S' || ch == 'U') {
		printf("I found a %c\n", ch);
	}

	printf("GPS %d %d sending message\n", x, y);

	bool update = bot.update();
	for (int i = 0; i < 2 && update; i++) {
		bot.stop();
		bot.delay(650);
		if (i == 1) {
			bot.emitter->send(message, sizeof(message));
		}
		update = bot.update();
	}
}

bool comp(vector<Point> first, vector<Point> second) {
	return (contourArea(first) < contourArea(second));
}

int findMinMax(vector<Point> points, bool findMax) {
#ifdef DEBUG_MODE
	printf("\nPoints.size: %zu, ", points.size());
#endif
	int max = points[0].y;
	int min = points[0].y;
	for (int i = 0; i < points.size(); i++) {
#ifdef DEBUG_MODE
		printf("%d, ", points[i].y);
#endif
		if (points[i].y > max) {
			max = points[i].y;
		}
		if (points[i].y < min) {
			min = points[i].y;
		}
	}
	if (findMax) return max + 1;
	else if (!findMax) return min - 1;
	printf("\n");
}

// HSU victim detection
char HSU(Mat roi) {
	vector<vector<Point>> contours;
	vector<vector<Point>> allContours;
	int slicedContours = 0;
	double area = 0;
	int topCropVal = 0;
	int botCropVal = 0;
	int botHeight = roi.rows / 4;
	double widthMult = 1;

	threshold(roi, roi, thresh, max_thresh, THRESH_BINARY_INV); // create thresholded image
	//findContours in whole ROI
	findContours(roi, allContours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
#ifdef DEBUG_MODE
	imshow("roi", roi);
	printf("\nROI Contour Count: %zu\n", allContours.size());
#endif
	//printf("\nContour Areas: ");

	if (allContours.size() == 3) {
		topCropVal = findMinMax(allContours[2], true); //find max
		botCropVal = findMinMax(allContours[0], false); //find min
		botHeight = roi.rows - botCropVal;
		widthMult = 0.85;
	}
#ifdef DEBUG_MODE
	printf("\nWidth: %d, Height: %d", roi.cols, roi.rows);
	printf("\ntopCropVal: %d, botCropVal: %d, botHeight: %d\n", topCropVal, botCropVal, botHeight);
#endif

	try {
		Mat cropTop(roi, Rect(0, topCropVal, roi.cols * widthMult, roi.rows / 4));
		Mat cropMid(roi, Rect(0, roi.rows * 3 / 8, roi.cols * widthMult, roi.rows / 4));
		Mat cropBot(roi, Rect(0, roi.rows * 3 / 4, roi.cols * widthMult, botHeight));

#ifdef DEBUG_MODE
		imshow("top_crop", cropTop);
		imshow("mid_crop", cropMid);
		imshow("bot_crop", cropBot);
#endif

		//Mat top(roi, Rect(0, 0, roi.cols, roi.rows / 4));
		//Mat mid(roi, Rect(0, roi.rows / 3, roi.cols, roi.rows / 4));
		//Mat bot(roi, Rect(0, roi.rows * 3 / 4, roi.cols, roi.rows / 4));

		//imshow("top_norm", top);
		//imshow("mid_norm", mid);
		//imshow("bot_norm", bot);

		// findContours on each third
		findContours(cropTop, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE); // find contours
		for (int i = 0; i < contours.size(); i++) {
			area += contourArea(contours[i]);
		}
		//printf("top contour: %f, ", area);
		//if (area < double(top.rows * top.cols * 0.05) || area > double(top.rows * top.cols * 0.3)) return 0;
		slicedContours = contours.size() * 100;
		findContours(cropMid, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE); // find contours
		area = 0;
		for (int i = 0; i < contours.size(); i++) {
			area += contourArea(contours[i]);
		}
		//printf("mid contour: %f, ", area);
		//printf("mid.rows: %d, ", mid.rows);
		//printf("mid.cols: %d", mid.cols);
		//if (area < double(mid.rows * mid.cols * 0.05) || area > double(mid.rows * mid.cols * 0.5)) {
		//	return true;
		//}
		slicedContours += contours.size() * 10;
		findContours(cropBot, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE); // find contours
		area = 0;
		for (int i = 0; i < contours.size(); i++) {
			area += contourArea(contours[i]);
		}
		//printf("bot contour: %f, ", area);
		//printf("bot.rows: %d, ", bot.rows);
		//printf("bot.cols: %d, \n", bot.cols);
		//if (area < double(bot.rows * bot.cols * 0.05) || area > double(bot.rows * bot.cols * 0.3)) return 0;
		slicedContours += contours.size();

		waitKey(1);

#ifdef DEBUG_MODE
		printf("SlicedContour: %d--------------------------\n", slicedContours);
#endif
		switch (slicedContours) {
		case 212: sendVictimSignal('H'); return 'H';
		case 111: sendVictimSignal('S'); return 'S';
		case 221: sendVictimSignal('U'); return 'U';
		default: return 0;
		}
	}
	catch (...) {
		printf("MATRIX ERROR!!!\n");
	}
}

// Victim/Hazard sign detection
char checkVisualVictim(Camera* cam) {
	//if (boardLoc(loc).victimChecked) return false; // don't repeatedly check the same tile

	Mat frame_HSV, frame_red, frame_yellow;
	Mat frame(cam->getHeight(), cam->getWidth(), CV_8UC4, (void*)cam->getImage());
	//imshow("frame", frame);
	if (frame.empty()) {
		cout << "Could not open or find the image!\n" << endl;
		return 0;
	}

	Mat clone = frame.clone();
	Mat drawing = frame.clone();
	// Convert from BGR to HSV colorspace
	cvtColor(frame, frame_HSV, COLOR_BGR2HSV);

	// Detect the object based on HSV Range Values
	inRange(frame_HSV, Scalar(low_H, low_S, low_V), Scalar(high_H, high_S, high_V), frame_red);
	inRange(frame_HSV, Scalar(low_Hy, low_S, low_V), Scalar(high_Hy, high_S, high_V), frame_yellow);

	//inRange(frame_HSV, Scalar(150, 60, 60), Scalar(180, 255, 255), frame_red);
	//inRange(frame_HSV, Scalar(0, 60, 60), Scalar(40, 255, 255), frame_yellow);

	Mat mask;
	vector<vector<Point>> contours;
	vector<vector<Point>> maskcontours;

	findContours(frame_red, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	if (contours.size() == 0) { // if see no red
		// remove blue background
		inRange(frame_HSV, Scalar(30, 0, 0), Scalar(160, 255, 255), mask);
		mask = ~mask;

		findContours(mask, maskcontours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

		if (maskcontours.size() > 0) {
			vector<Point> largest = *max_element(maskcontours.begin(), maskcontours.end(), comp);
			Rect boundRect = boundingRect(largest);
			//printf("TopLeft X: %d, Y: %d\n", boundRect.x, boundRect.y);
			Mat roi(frame, boundRect);
			//imshow("roi", roi);
			Mat thresholded;
			cvtColor(roi, roi, COLOR_BGR2GRAY);
			threshold(roi, roi, 120, 255, THRESH_BINARY);

			if (maskcontours.size() == 1) {
				findContours(roi, maskcontours, RETR_TREE, CHAIN_APPROX_SIMPLE);
			}
			else {
				findContours(roi, maskcontours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
			}

			//printf("\n# of Contours: %d\n", maskcontours.size());
			if (maskcontours.size() > 0) {
				largest = *max_element(maskcontours.begin(), maskcontours.end(), comp);
				RotatedRect rotateRect = minAreaRect(largest);
				boundRect = boundingRect(largest);
				double area = roi.rows * roi.cols;

				double rat = (float)roi.cols / roi.rows;
#ifdef DEBUG_MODE
				printf("angle %f, rows %d, cols %d, ratio %f, framerows %d\n", rotateRect.angle, roi.rows, roi.cols, rat, frame.rows);
#endif

				//imshow("roi", roi);
				//waitKey(1);

				// poison or corrosive hazard signs
				if (rotateRect.angle < 52 && rotateRect.angle > 38 && (float)roi.cols / (float)roi.rows > 0.7 && (float)roi.cols / (float)roi.rows < 1.2 && roi.rows > frame.rows / 2) {
					//printf("largest %f %f\n", contourArea(largest), contourArea(largest) / area);
					Mat black;
					double blackarea = 0;
					black = ~roi;
					black = black(Rect(0, black.rows / 2, black.cols, black.rows / 2));

					//imshow("black", black);
					//waitKey(1);
					//findContours(black, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
#ifdef DEBUG_MODE
					printf("contour count %I64u\n", maskcontours.size());
#endif
					//printf("area %f\n", area);
					for (int i = 0; i < maskcontours.size(); i++) {
						blackarea += contourArea(maskcontours[i]);
					}

					// get gps
					//PosX = gps->getValues()[0] * 100;
					//PosZ = gps->getValues()[2] * 100;

#ifdef DEBUG_MODE
					printf("blackarea %f, area %f, largest %f\n", blackarea, area, contourArea(largest));
					printf("black/area: %f\n", blackarea / area);
#endif

					if ((blackarea / area > 0.32 && blackarea / area < 0.44 && maskcontours.size() >= 2)
						|| (maskcontours.size() >= 8 && maskcontours.size() <= 12 && 
							blackarea / area > 0.68 && blackarea / area < 0.92)) {
						printf("CORROSIVE\n");
						sendVictimSignal('C');
						//changeMessage(PosX, PosZ, 'C');
						//boardLoc(loc).victimChecked = true;
						return 'C';
					}
					else if (maskcontours.size() >= 5 && maskcontours.size() <= 15 &&
						blackarea / area > 0.51 && blackarea / area < 0.77){
						printf("POISON\n");
						sendVictimSignal('P');
						//changeMessage(PosX, PosZ, 'P');
						//boardLoc(loc).victimChecked = true;
						return 'P';
					}
				}
				// HSU
				vector<vector<Point>> roiContours;
				findContours(roi, roiContours, RETR_TREE, CHAIN_APPROX_SIMPLE);
				vector<Point>biggest = *max_element(roiContours.begin(), roiContours.end(), comp);
				RotatedRect rectRotate = minAreaRect(biggest);
				Rect rectBound = boundingRect(biggest);
				float hsuArea = 0;
				for (int i = 0; i < roiContours.size(); i++) {
					hsuArea += contourArea(roiContours[i]);
				}
				float roicol = roi.rows * roi.cols;
				float ratio = hsuArea / roicol;
				//printf("area: %f, roicol: %f, ratio: %f\n", hsuArea, roicol, ratio);
				if (ratio < 1) {
					//printf("RATIO TOO LOW\n");
					return false;
				}

				bool temp1 = (float)roi.cols / roi.rows >= 0.5 && (float)roi.cols /roi.rows < 1.5;
				bool temp2 = roi.rows > 20 && roi.cols < 10 && (float)roi.cols / roi.rows >= 0.1 && (float)roi.cols / roi.rows < 0.35;

				if (rotateRect.angle > 85 && rotateRect.angle < 95 && roi.rows > frame.rows / 2 && (temp1) ) 
				{
					char hsu = HSU(roi);
					if (hsu) {
						return hsu;
					}
					else {
						return 0;
					}
					//if (roundAngle(angle) == -1) roi = roi(Rect(0, 5, roi.cols, roi.rows - 10));
					roi = roi(Rect(2, 0, roi.cols - 4, roi.rows)); // resize

					// get gps
					//PosX = gps->getValues()[0] * 100;
					//PosZ = gps->getValues()[2] * 100;

				}
			}
		}
	}
	// peroxide or flammable gas
	else {
		for (int i = 0; i < contours.size(); i++) {
			Rect roi = boundingRect(contours[i]);

			double width = (double)roi.width;
			double height = (double)roi.height;
			
#ifdef DEBUG_MODE
			printf("width % f height %f, ", width, height);
			printf("width/height: %f\n", width / height);
#endif

			//if (( width / height > 0.4 && width / height < 2.1 && height > 30 && width > 30) ||
			//	(width > 30 && width < 100 && height > 15 && height < 100 && (width / height > 0.9 && width / height < 2.1))) {
						//flammable
			bool cond1 = width / height > 0.4 && width / height < 0.65 && width > 10 && height > 17;
			//organic peroxide
			bool cond2 = width / height > 0.8 && width / height < 2.1 && width > 10 && height > 7 && height < 17;
			if(cond1 || cond2){
				findContours(frame_yellow, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
				printf("seeYellow: %d\n", contours.size() != 0);
				if (contours.size() == 0) { //if no yellow
					printf("FLAMMABLE\n");
					sendVictimSignal('F');
					//changeMessage(PosX, PosZ, 'F');
					//boardLoc(loc).victimChecked = true;
					return 'F';
				}
				else if(contours.size() != 0){
					printf("ORGANIC PEROXIDE\n");
					sendVictimSignal('O');
					//changeMessage(PosX, PosZ, 'O');
					//boardLoc(loc).victimChecked = true;
					return 'O';
				}
			}
		}
	}
	return 0;
}
#include "victim.h"

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

// HSU victim detection
char HSU(Mat roi) {
	vector<vector<Point>> contours;
	unsigned char slicedContours = 0;
	double area = 0;

	threshold(roi, roi, thresh, max_thresh, THRESH_BINARY_INV); // create thresholded image

	Mat top(roi, Rect(0, 0, roi.cols, roi.rows / 4));
	Mat mid(roi, Rect(0, roi.rows / 3, roi.cols, roi.rows / 4));
	Mat bot(roi, Rect(0, roi.rows * 3 / 4, roi.cols, roi.rows / 4));

	// findContours on each third
	findContours(top, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE); // find contours
	for (int i = 0; i < contours.size(); i++) {
		area += contourArea(contours[i]);
	}
	//printf("top contour: %f\n", area);
	if (area < double(top.rows * top.cols * 0.05) || area > double(top.rows * top.cols * 0.3)) return 0;
	slicedContours = (unsigned char)contours.size() * 100;
	findContours(mid, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE); // find contours
	area = 0;
	for (int i = 0; i < contours.size(); i++) {
		area += contourArea(contours[i]);
	}
	//printf("mid contour: %f\n", area);
	if (area < double(mid.rows * mid.cols * 0.05) || area > double(mid.rows * mid.cols * 0.3)) return 0;
	slicedContours += (unsigned char)contours.size() * 10;
	findContours(bot, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE); // find contours
	area = 0;
	for (int i = 0; i < contours.size(); i++) {
		area += contourArea(contours[i]);
	}
	//printf("bot contour: %f\n", area);
	if (area < double(bot.rows * bot.cols * 0.05) || area > double(bot.rows * bot.cols * 0.3)) return 0;
	slicedContours += (unsigned char)contours.size();

	//imshow("hsu", roi);
	//imshow("top", top);
	//imshow("mid", mid);
	//imshow("bot", bot);
	//waitKey(1);

	switch (slicedContours) {
		case 212: return 'H';
		case 111: return 'S';
		case 221: return 'U';
		default: return 0;
	}
}

bool comp(vector<Point> first, vector<Point> second) {
	return (contourArea(first) < contourArea(second));
}

// Victim/Hazard sign detection
bool checkVisualVictim(Camera *cam) {
	//if (boardLoc(loc).victimChecked) return false; // don't repeatedly check the same tile

	Mat frame_HSV, frame_red, frame_yellow;
	Mat frame(cam->getHeight(), cam->getWidth(), CV_8UC4, (void *)cam->getImage());

	if (frame.empty()) {
		cout << "Could not open or find the image!\n" << endl;
		return false;
	}

	Mat clone = frame.clone();
	Mat drawing = frame.clone();
	// Convert from BGR to HSV colorspace
	cvtColor(frame, frame_HSV, COLOR_BGR2HSV);
	// Detect the object based on HSV Range Values
	inRange(frame_HSV, Scalar(low_H, low_S, low_V), Scalar(high_H, high_S, high_V), frame_red);
	inRange(frame_HSV, Scalar(low_Hy, low_S, low_V), Scalar(high_Hy, high_S, high_V), frame_yellow);

	Mat mask;
	vector<vector<Point>> contours;
	vector<vector<Point>> maskcontours;

	findContours(frame_red, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	//int angle = (int)getAngle();

	if (contours.size() == 0) { // if see no red
		// remove blue background
		inRange(frame_HSV, Scalar(30, 0, 0), Scalar(160, 255, 255), mask);
		mask = ~mask;

		findContours(mask, maskcontours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

		if (maskcontours.size() > 0) {
			vector<Point> largest = *max_element(maskcontours.begin(), maskcontours.end(), comp);
			Rect boundRect = boundingRect(largest);
			Mat roi(frame, boundRect);
			Mat thresholded;
			cvtColor(roi, roi, COLOR_BGR2GRAY);
			threshold(roi, roi, 120, 255, THRESH_BINARY);
			findContours(roi, maskcontours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

			if (maskcontours.size() > 0) {
				largest = *max_element(maskcontours.begin(), maskcontours.end(), comp);
				RotatedRect rotateRect = minAreaRect(largest);
				boundRect = boundingRect(largest);
				double area = roi.rows * roi.cols;

				//printf("%f\n", contourArea(largest));
				//printf("angle %f rows %d cols %d\n", rotateRect.angle, roi.rows, roi.cols);

				//imshow("roi", roi);
				//waitKey(1);

				// poison or corrosive hazard signs
				if (rotateRect.angle < 52 && rotateRect.angle > 38 && (float)roi.cols / (float)roi.rows > 0.6 && (float)roi.cols / (float)roi.rows < 1.2 && roi.rows > frame.rows / 2) {
					//printf("largest %f %f\n", contourArea(largest), contourArea(largest) / area);
					Mat black;
					double blackarea = 0;
					black = ~roi;
					black = black(Rect(0, black.rows / 2, black.cols, black.rows / 2));

					//imshow("black", black);
					//waitKey(1);
					findContours(black, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
					//printf("contour count %d\n", contours.size());
					//printf("area %f\n", area);
					for (int i = 0; i < contours.size(); i++) {
						blackarea += contourArea(contours[i]);
					}

					// get gps
					//PosX = gps->getValues()[0] * 100;
					//PosZ = gps->getValues()[2] * 100;

					//printf("area %f\n", blackarea);

					if (blackarea / area > 0.09 && blackarea / area < 0.2 && contours.size() > 10) {
						printf("poison\n");
						//changeMessage(PosX, PosZ, 'P');
						//boardLoc(loc).victimChecked = true;
						return true;
					}

					if (blackarea / area > 0.2 && blackarea / area < 0.5 && contours.size() < 10 && contours.size() > 3) {
						printf("corrosive\n");
						//changeMessage(PosX, PosZ, 'C');
						//boardLoc(loc).victimChecked = true;
						return true;
					}
				}
				// HSU
				if (rotateRect.angle > 85 &&
					rotateRect.angle < 95 &&
					roi.rows > frame.rows / 2 &&
					(((float)roi.cols / (float)roi.rows > 1.0 && (float)roi.cols / (float)roi.rows < 1.5) ||
					 (/*roundAngle(angle) == -1 && */ (float)roi.cols / (float)roi.rows > 0.7 && (float)roi.cols / (float)roi.rows < 1.5))) {

					//if (roundAngle(angle) == -1) roi = roi(Rect(0, 5, roi.cols, roi.rows - 10));
					roi = roi(Rect(2, 0, roi.cols - 4, roi.rows)); // resize

					// get gps
					//PosX = gps->getValues()[0] * 100;
					//PosZ = gps->getValues()[2] * 100;

					// detect letter
					char letter = HSU(roi);
					if (letter) { // send to erebus
						printf("I found a %c!\n", letter);
						//changeMessage(PosX, PosZ, letter);
						//boardLoc(loc).victimChecked = true;
						return true;
					}
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

			//printf("width %f height %f\n", width, height);

			if ((/*roundAngle(angle) == -1 && */ width / height > 0.4 && width / height < 2.1 && height > 30 && width > 30) ||
				(width > 30 && width < 100 && height > 15 && height < 100 && (width / height > 0.9 && width / height < 2.1))){
				// get gps
				//PosX = gps->getValues()[0] * 100;
				//PosZ = gps->getValues()[2] * 100;

				findContours(frame_yellow, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
				if (contours.size() == 0) { //if no yellow
					printf("flammable\n");
					//changeMessage(PosX, PosZ, 'F');
					//boardLoc(loc).victimChecked = true;
					return true;
				}
				else {
					printf("organic peroxide\n");
					//changeMessage(PosX, PosZ, 'O');
					//boardLoc(loc).victimChecked = true;
					return true;
				}
			}
		}
	}
	return false;
}
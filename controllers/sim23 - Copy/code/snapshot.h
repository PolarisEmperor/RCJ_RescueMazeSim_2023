#ifndef __SNAPSHOT_HPP__
#define __SNAPSHOT_HPP__

#include <vector>
#include "point.h"

class Snapshot {

public:
    Snapshot();
    Snapshot(std::vector<Point> pointList, unsigned short snapshotNumber, bool snapshotVisited);
    bool operator<(const Snapshot& snapshot) const;
    float calculateSnapshotAngle();
    unsigned short calculateSnapshotAccuracy();

    std::vector<Point> points;
    unsigned short snapNum;
    unsigned short snapAccuracy;
    float snapAngle;
    bool snapVisited;

};

#endif#pragma once

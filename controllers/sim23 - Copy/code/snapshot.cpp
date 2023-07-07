#include "snapshot.h"

Snapshot::Snapshot() {
    snapNum = 0;
    snapAccuracy = 0;
    snapAngle = 0;
    snapVisited = false;
}

Snapshot::Snapshot(std::vector<Point> pts, unsigned short n, bool v) {
    points = pts;
    snapNum = n;
    snapVisited = v;
    snapAngle = calculateSnapshotAngle();
    snapAccuracy = calculateSnapshotAccuracy();
}

bool Snapshot::operator<(const Snapshot& s) const {
    return this->snapNum < s.snapNum;
}

float Snapshot::calculateSnapshotAngle() {
    float angle = 0;
    for (auto p : points) {
        angle += p.angle;
    }
    return angle / points.size();
}

unsigned short Snapshot::calculateSnapshotAccuracy() {
    unsigned int accuracy = 0;
    for (auto p : points) {
        accuracy += p.distance;
    }
    return accuracy / points.size();
}

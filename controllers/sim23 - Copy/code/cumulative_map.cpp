#include <math.h>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

#include "cumulative_map.h"
#include "map_tile.h"

#define LIDAR_RESOLUTION 512 

TileMap::TileMap() {
    tileCount = 0;
}

MapTile& TileMap::getTile(std::pair<int, int>& t) {
    return tiles[t.first][t.second];
}

void TileMap::createTile(std::pair<int, int>& t, MapTile tile) {
    if (!tiles.count(t.first)) {
        tiles[t.first] = std::unordered_map<int, MapTile>();
    }
    if (!tiles[t.first].count(t.second)) {
        tiles[t.first][t.second] = tile;
    }
}

bool TileMap::tileExists(std::pair<int, int>& t) {
    if (tiles.count(t.first)) {
        if (tiles[t.first].count(t.second))
            return true;
    }
    return false;
}

void TileMap::addSnapshot(const float* distList, const float GPSx, const float GPSy, const float IMUa) {
    std::vector<Point> pointList;

    //convert distance list to vector of points
    for (int i = 0; i < LIDAR_RESOLUTION; i++) {
        if (!isnormal(distList[i]))
            continue;

        float pointAngle = (i * ((2 * 3.14159265) / LIDAR_RESOLUTION));
        pointList.push_back(Point(GPSx, GPSy, IMUa, distList[i], pointAngle, false));
    }

    //TODO - check backward: 512 and 0 may be part of same snap
    std::pair<int, int> previousTile = MapTile::calculateTile(pointList[0]);
    int start = 0;
    for (int i = 1; i < pointList.size(); i++) {
        if (MapTile::calculateTile(pointList[i]) != previousTile) {
            Snapshot s = Snapshot(std::vector<Point>(pointList.begin() + start, pointList.begin() + (i - 1)), snapshotCount, false);

            if (tileExists(previousTile)) {
                MapTile& t = getTile(previousTile);
                t.addSnapshotToTile(s);
            }
            else {
                createTile(previousTile, MapTile(s));
            }

            start = i;
            previousTile = MapTile::calculateTile(pointList[i]);
        }
    }

    snapshotCount++;
}
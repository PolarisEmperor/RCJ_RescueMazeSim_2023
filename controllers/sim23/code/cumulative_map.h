#ifndef __CUMULATIVE_MAP_HPP__
#define __CUMULATIVE_MAP_HPP__

#include <unordered_map>
#include "map_tile.h"

class TileMap {

public:
    TileMap();

    MapTile& getTile(std::pair<int, int>& tile);
    void createTile(std::pair<int, int>& tile, MapTile theTile);
    bool tileExists(std::pair<int, int>& tile);
    void addSnapshot(const float* LiDAR_scan, const float GPSx, const float GPSy, const float IMUa);

    std::unordered_map<int, std::unordered_map<int, MapTile> > tiles;
    unsigned short tileCount;
    unsigned short snapshotCount;
};


#endif
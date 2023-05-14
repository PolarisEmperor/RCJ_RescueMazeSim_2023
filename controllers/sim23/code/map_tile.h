#ifndef __MAP_TILE_HPP__
#define __MAP_TILE_HPP__

#include <set>
#include "snapshot.h"

class Tile {

public:
    Tile();
    Tile(Snapshot snapshot);
    bool addSnapshotToTile(Snapshot snapshot);
    static std::pair<int, int> calculateTile(Point point);

    std::set<Snapshot> snapshots;
    unsigned short tileAccuracy;
    bool tileVisited;

};

#endif
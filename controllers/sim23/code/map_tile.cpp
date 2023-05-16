#include "map_tile.h"
#include "point.h"
#include <math.h>
#define TILE_SIZE 0.2

MapTile::MapTile() {
    tileAccuracy = 0;
    tileVisited = false;
}

MapTile::MapTile(Snapshot snap) {
    snapshots.insert(snap);
}

// returns the tile that a points belongs to
std::pair<int, int> MapTile::calculateTile(Point p) {
    return std::pair<int, int>(trunc(p.x / TILE_SIZE), trunc(p.y / TILE_SIZE));
}

bool MapTile::addSnapshotToTile(Snapshot s) {
    
    return true;
}

#include "map_tile.h"
#include "point.h"
#include <math.h>
#define TILE_SIZE 0.2

Tile::Tile() {
    tileAccuracy = 0;
    tileVisited = false;
}

Tile::Tile(Snapshot snap) {
    snapshots.insert(snap);
}

// returns the tile that a points belongs to
std::pair<int, int> Tile::calculateTile(Point p) {
    return std::pair<int, int>(trunc(p.x / TILE_SIZE), trunc(p.y / TILE_SIZE));
}

bool Tile::addSnapshotToTile(Snapshot s) {
    
    return true;
}

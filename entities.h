#ifndef ENTITIES_H
#define ENTITIES_H

#include <boost/shared_ptr.hpp>
#include "constants.h"
#include "hexgrid.h"

class Map;
class HexTile;
class Track;

class Track {
    HexTile* hexTile;
    ptHexGrid::Direction startDir;
    bool exits[3]; // 0=left-curve, 1=straight, 2=right-curve
public:
    Track(HexTile* _hexTile, ptHexGrid::Direction _startDir, bool _exits[3]);
    void drawHere();
};

enum class TileType { Dirt, Grass, Wall, Water };

class HexTile {
    Map *map;
    TileType tileType;
    vector2i axialPos;
    boost::shared_ptr<Track> track;
public:
    HexTile();
    HexTile(TileType _tileType, vector2i _axialPos, Map *_map);
    Map *refMap();
    boost::shared_ptr<Track> refTrack();
    void setTrack(boost::shared_ptr<Track> track);
    vector2i coAxialPos();
    vector2f coMapPos();
    void drawHere();
    void drawAtPos();
};

class Map {
    float tileCircumradius;
    vector2i dimensions;
    HexTile tiles[MAX_MAP_DIM][MAX_MAP_DIM];
public:
    Map(float _tileCircumradius, int width, int height);
    void generate();
    HexTile* refTile(vector2i axial);
    // use of 'coVar' convention to indicate copies (to help track mutability and side effects)
    float coTileCircumradius(); // 'copy of circumRadius'
    float coScale(); // helpful synonym for ^
    float coTileLongwidth();
    float coTileShortwidth();
    void drawHere();
};

#endif
#ifndef ENTITIES_H
#define ENTITIES_H

#include <boost/shared_ptr.hpp>
#include "constants.h"
#include "hexgrid.h"

class GameMap;
class HexTile;
class Track;

enum class TrackCurveType {Straight, CurveLeft, CurveRight };

class Track {
    HexTile* hexTile;
    ptHexGrid::Direction inDir;
    bool exits[3]; // 0=left-curve, 1=straight, 2=right-curve
    void init(HexTile* _hexTile, ptHexGrid::Direction _startDir);
public:
    Track(HexTile* _hexTile, ptHexGrid::Direction _startDir);
    Track(HexTile* _hexTile, ptHexGrid::Direction _startDir, bool _exits[3]);
    int numExits();
    ptHexGrid::Direction coInDir();
    ptHexGrid::Direction calculateOutDir(TrackCurveType curveType);
    void addExit(TrackCurveType curveType);
    std::unique_ptr<TrackCurveType> getFirstExitCurveType();
    HexTile* refHexTile();
    void drawHere();
};

enum class TileType { Dirt, Grass, Wall, Water };

class HexTile {
    GameMap *map;
    TileType tileType;
    vector2i axialPos;
    boost::shared_ptr<Track> track;
public:
    HexTile();
    HexTile(TileType _tileType, vector2i _axialPos, GameMap *_map);
    GameMap *refMap();
    boost::shared_ptr<Track> refTrack();
    void setTrack(boost::shared_ptr<Track> track);
    vector2i coAxialPos();
    vector2f coMapPos();
    void drawHere();
    void drawAtPos();
};

class GameMap {
    float tileCircumradius;
    vector2i dimensions;
    HexTile tiles[MAX_MAP_DIM][MAX_MAP_DIM];
public:
    GameMap(float _tileCircumradius, int width, int height);
    void generate();
    HexTile* refTile(vector2i axial);
    // use of 'coVar' convention to indicate copies (to help track mutability and side effects)
    float coTileCircumradius(); // 'copy of circumRadius'
    float coScale(); // helpful synonym for ^
    float coTileLongwidth();
    float coTileShortwidth();
    void drawHere();
};

class Cart {
    boost::shared_ptr<Track> currentTrack;
    float trackPosition;
    int trackExit;
    float speed;
public:
    Cart(boost::shared_ptr<Track> _currentTrack, float _trackPosition, int trackExit);
    boost::shared_ptr<Track> refTrack();
    void go();
    void drawHere();
    void drawAtPos();
};

#endif
#include <SFML/Graphics.hpp>
#include <math.h>
#include <iostream>
#include <boost/shared_ptr.hpp>
#include "myvectors.h"
#include "constants.h"
#include "hexgrid.h"
#include "entities.h"
#include "drawing.h"

using namespace std;

Track::Track(HexTile* _hexTile, ptHexGrid::Direction _startDir, bool _exits[3]) {
    hexTile = _hexTile;
    startDir = _startDir;
    for (int i=0; i<sizeof(exits); i++) {
        exits[i] = _exits[i];
    }
}

GameMap::GameMap(float _tileCircumradius, int width, int height) {
    if ( width >= MAX_MAP_DIM || height >= MAX_MAP_DIM) {
        throw "Map dimensions exceed max";
    }

    dimensions = vector2i(width, height);
    tileCircumradius = _tileCircumradius;
    // cout << width << "," << height << endl;
}
void GameMap::generate() {
    for (int q=0; q < dimensions.x; q++) {
        for (int r=0; r < dimensions.y; r++) {
            // cout << q << "," << r << endl;
            TileType type = TileType::Dirt;

            if ((q == 0 || r%4 == 0) && q !=5) {
                type = TileType::Wall;
            }
            if (r + q == 25) {
                type = TileType::Water;
            }

            tiles[q][r] = HexTile(type, vector2i(q,r), this);
        }
    }
}
HexTile* GameMap::refTile(vector2i axial) {
    if (axial.x >= dimensions.x || axial.y >= dimensions.y) {
        throw "axial pos out of map bounds";
    }
    return &(tiles[axial.x][axial.y]);
}
// use of 'coVar' convention to indicate copies (to help track mutability and side effects)
float GameMap::coTileCircumradius() { // 'copy of circumRadius'
    return tileCircumradius;
}
float GameMap::coScale() { // helpful synonym
    return coTileCircumradius();
}
float GameMap::coTileLongwidth() {
    return tileCircumradius * 2;
}
float GameMap::coTileShortwidth() {
    return coTileLongwidth() * 3.0/4.0;
}

HexTile::HexTile(TileType _tileType, vector2i _axialPos, GameMap *_map) {
    tileType = _tileType;
    axialPos = _axialPos;
    map = _map;
}
HexTile::HexTile() {}

GameMap* HexTile::refMap() {
    return map;
}
boost::shared_ptr<Track> HexTile::refTrack() {
    return track;
}
void HexTile::setTrack(boost::shared_ptr<Track> newTrack) {
    track = newTrack;
}
vector2i HexTile::coAxialPos() {
    return axialPos;
}
vector2f HexTile::coMapPos() {
    return ptHexGrid::axialToReal(axialPos, map->coTileCircumradius());
    return vector2f(
        map->coTileLongwidth() * (3./4 * axialPos.x),
        map->coTileLongwidth() * (sqrt(3)/4 * axialPos.x  +  sqrt(3)/2 * axialPos.y)
    );
}

GameMap gameMap(32, 50, 50);
HexTile* currentTile;
ptHexGrid::Direction currentDir;

void layTrack(TrackCurveType curveType) {
    ptHexGrid::Direction nextTileDir;
    bool trackExits[] = { false, false, false};

    switch (curveType) {
        case TrackCurveType::Straight:
            nextTileDir = currentDir;
            trackExits[1] = true;
            break;
        case TrackCurveType::CurveLeft:
            nextTileDir = ptHexGrid::dirTurnedCCWOnce(currentDir);
            trackExits[0] = true;
            break;
        case TrackCurveType::CurveRight:
            nextTileDir = ptHexGrid::dirTurnedCWOnce(currentDir);
            trackExits[2] = true;
            break;
    }

    currentTile->setTrack(boost::shared_ptr<Track>(new Track(currentTile, currentDir, trackExits)));
    currentTile = gameMap.refTile(ptHexGrid::getNeighborInDirection(currentTile->coAxialPos(), nextTileDir));
    currentDir = nextTileDir;
}

int main (int argc, char **argv) {
    gameMap.generate();

    currentDir = ptHexGrid::Right;
    currentTile = gameMap.refTile(vector2i(1, 22));

    // cout << hexTile1.refMap()->coTileLongwidth() << endl << endl;

    // cout << hexTile1.coMapPos().x << "," << hexTile1.coMapPos().y << endl;
    // cout << hexTile2.coMapPos().x << "," << hexTile2.coMapPos().y << endl;
    // cout << hexTile3.coMapPos().x << "," << hexTile3.coMapPos().y << endl;

    // sf::RenderWindow window(sf::VideoMode(1920, 1080), "HexStuff", sf::Style::Fullscreen);
    // unfortunately renders on the non-streaming window
    sf::RenderWindow window(sf::VideoMode(1800, 950), "HexStuff", sf::Style::Close | sf::Style::Titlebar);

    // Your own custom OpenGL setup calls here
    // There's no additional code needed, unless you want to mix SFML drawing and raw OpenGL

    sf::Event event;

    while (window.isOpen()) { // This is the program's main loop

        while (window.pollEvent(event)) { // Event handling
            switch (event.type) {
                case sf::Event::Closed:
                    window.close();
                    break;
                case sf::Event::KeyPressed:
                    switch (event.key.code) {
                        case sf::Keyboard::Up:
                            layTrack(TrackCurveType::Straight);
                            break;
                        case sf::Keyboard::Left:
                            layTrack(TrackCurveType::CurveLeft);
                            break;
                        case sf::Keyboard::Right:
                            layTrack(TrackCurveType::CurveRight);
                            break;
                    }
                    break;
            }
        }
        window.clear(); // Clear the buffer

        draw(&gameMap);

        window.display(); // Update window contents
    }
    return 0;
}

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

Map::Map(float _tileCircumradius, int width, int height) {
    if ( width >= MAX_MAP_DIM || height >= MAX_MAP_DIM) {
        throw "Map dimensions exceed max";
    }

    dimensions = vector2i(width, height);
    tileCircumradius = _tileCircumradius;
    // cout << width << "," << height << endl;
}
void Map::generate() {
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
HexTile* Map::refTile(vector2i axial) {
    if (axial.x >= dimensions.x || axial.y >= dimensions.y) {
        throw "axial pos out of map bounds";
    }
    return &(tiles[axial.x][axial.y]);
}
// use of 'coVar' convention to indicate copies (to help track mutability and side effects)
float Map::coTileCircumradius() { // 'copy of circumRadius'
    return tileCircumradius;
}
float Map::coScale() { // helpful synonym
    return coTileCircumradius();
}
float Map::coTileLongwidth() {
    return tileCircumradius * 2;
}
float Map::coTileShortwidth() {
    return coTileLongwidth() * 3.0/4.0;
}

HexTile::HexTile(TileType _tileType, vector2i _axialPos, Map *_map) {
    tileType = _tileType;
    axialPos = _axialPos;
    map = _map;
}
HexTile::HexTile() {}

Map* HexTile::refMap() {
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



int main (int argc, char **argv) {
    Map map(32, 50, 50);
    map.generate();
    HexTile* testTile = map.refTile(vector2i(1, 22));
    bool trackExits[] = {true, true, true};
    testTile->setTrack(boost::shared_ptr<Track>(new Track(testTile, ptHexGrid::Right, trackExits)));

    // cout << hexTile1.refMap()->coTileLongwidth() << endl << endl;

    // cout << hexTile1.coMapPos().x << "," << hexTile1.coMapPos().y << endl;
    // cout << hexTile2.coMapPos().x << "," << hexTile2.coMapPos().y << endl;
    // cout << hexTile3.coMapPos().x << "," << hexTile3.coMapPos().y << endl;

    // sf::RenderWindow window(sf::VideoMode(1920, 1080), "HexStuff", sf::Style::Fullscreen);
    // unfortunately renders on the non-streaming window
    sf::RenderWindow window(sf::VideoMode(1800, 950), "HexStuff", sf::Style::Close | sf::Style::Titlebar);

    // Your own custom OpenGL setup calls here
    // There's no additional code needed, unless you want to mix SFML drawing and raw OpenGL

    sf::Event e;

    while (window.isOpen()) { // This is the program's main loop

        while (window.pollEvent(e)) { // Event handling
            switch (e.type) {
                case sf::Event::Closed:
                    window.close();
                    break;
                case sf::Event::KeyPressed:
                    break;
            }
        }
        window.clear(); // Clear the buffer

        draw(&map);

        window.display(); // Update window contents
    }
    return 0;
}

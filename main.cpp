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

ptHexGrid::Direction calculateCurveOutDir(ptHexGrid::Direction inDir, TrackCurveType curveType) {
    switch (curveType) {
        case TrackCurveType::CurveLeft:
            return ptHexGrid::dirTurnedCCWOnce(inDir);
            break;
        case TrackCurveType::Straight:
            return inDir;
            break;
        case TrackCurveType::CurveRight:
            return ptHexGrid::dirTurnedCWOnce(inDir);
            break;
    }
}
TrackCurveType flipTrackCurve(TrackCurveType curveType) {
    switch (curveType) {
        case TrackCurveType::CurveLeft:
            return TrackCurveType::CurveRight;
            break;
        case TrackCurveType::Straight:
            return TrackCurveType::Straight;
            break;
        case TrackCurveType::CurveRight:
            return TrackCurveType::CurveLeft;
            break;
    }
}

int Track::numExits() {
    int count = 0;
    for (int i=0; i<3; i++) {
        if (exits[i]) count++;
    }
    return count;
}
void Track::init(HexTile* _hexTile, ptHexGrid::Direction _startDir) {
    hexTile = _hexTile;
    inDir = _startDir;
}
Track::Track(HexTile* _hexTile, ptHexGrid::Direction _startDir, bool _exits[3]) {
    init(_hexTile, _startDir);
    for (int i=0; i<sizeof(exits); i++) {
        exits[i] = _exits[i];
    }
}
Track::Track(HexTile* _hexTile, ptHexGrid::Direction _startDir) {
    init(_hexTile, _startDir);
    for (int i=0; i<sizeof(exits); i++) {
        exits[i] = 0;
    }
}
ptHexGrid::Direction Track::coInDir() {
    return inDir;
}
ptHexGrid::Direction Track::calculateOutDir(TrackCurveType curveType) {
    return calculateCurveOutDir(inDir, curveType);
}
int curveTypeToIter(TrackCurveType curveType) {
    switch (curveType) {
        case TrackCurveType::CurveLeft:
            return 0;
            break;
        case TrackCurveType::Straight:
            return 1;
            break;
        case TrackCurveType::CurveRight:
            return 2;
            break;
    }
}
void Track::addExit(TrackCurveType curveType) {
    exits[curveTypeToIter(curveType)] = true;
}
std::unique_ptr<TrackCurveType> Track::getFirstExitCurveType() {
    if (exits[0]) return std::unique_ptr<TrackCurveType>(new TrackCurveType(TrackCurveType::CurveLeft));
    if (exits[1]) return std::unique_ptr<TrackCurveType>(new TrackCurveType(TrackCurveType::Straight));
    if (exits[2]) return std::unique_ptr<TrackCurveType>(new TrackCurveType(TrackCurveType::CurveRight));
    return std::unique_ptr<TrackCurveType>(); // return empty if no exits
}
HexTile* Track::refHexTile() {
    return hexTile;
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
            TileType type = TileType::Clear;

            if (q == 0 || r == 0 || q == 15 || r == 15) {
                type = TileType::Wall;
            }

            tiles[q][r] = HexTile(type, vector2i(q,r), this);
        }
    }

    //generate river
    vector2i axial(0, 15);
    ptHexGrid::Direction dir = ptHexGrid::Direction::UpRight;
    while (this->axialIsInBounds(axial)) {
        tiles[axial.x][axial.y] = HexTile(TileType::Water, axial, this);
        
        // maybe change dir
        
        int turnRoll = rand() % 20;
        if (turnRoll < 5) dir = ptHexGrid::dirTurnedCWOnce(dir);
        else if (turnRoll < 10) dir = ptHexGrid::dirTurnedCCWOnce(dir);
        axial += ptHexGrid::directionToAxial(dir);
    }

    //sprinkle ore
    for (int i=0; i<20; i++) {
        axial = vector2i(rand() % 16, rand() % 16);
        if (tiles[axial.x][axial.y].getTileType() != TileType::Water) {
            tiles[axial.x][axial.y] = HexTile(TileType::Ore, axial, this);
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
bool GameMap::axialIsInBounds(vector2i axial) {
    return !(axial.x < 0 || axial.y < 0 || axial.x >= this->dimensions.x || axial.y >= this->dimensions.y);
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
TileType HexTile::getTileType() {
    return tileType;
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

GameMap gameMap(32, 16, 16);
HexTile* currentTile;
ptHexGrid::Direction currentDir;

bool *flipExits(bool exits[3]) {
    // but actually this should either modify the array (yuck) or use a vector.
}

boost::shared_ptr<Track> tryCombineTracks(boost::shared_ptr<Track> existingTrack, ptHexGrid::Direction inDir, TrackCurveType curveType) {
    if (std::unique_ptr<TrackCurveType> firstExitCurveTypePtr = existingTrack->getFirstExitCurveType()) {
        TrackCurveType firstExitCurveType = *firstExitCurveTypePtr;
        if (existingTrack->numExits() == 1) {
            if (existingTrack->coInDir() == inDir) {
                existingTrack->addExit(curveType);
                return existingTrack;
            }
            else if (existingTrack->coInDir() == ptHexGrid::reverseDirection(calculateCurveOutDir(inDir, curveType))) {
                existingTrack->addExit(flipTrackCurve(curveType));
                return existingTrack;
            }
            else if (existingTrack->calculateOutDir(firstExitCurveType) == ptHexGrid::reverseDirection(inDir)) {
                boost::shared_ptr<Track> newTrack(new Track(existingTrack->refHexTile(), inDir));
                newTrack->addExit(flipTrackCurve(firstExitCurveType));
                newTrack->addExit(curveType);
                return newTrack;
            }
            else if (existingTrack->calculateOutDir(firstExitCurveType) == calculateCurveOutDir(inDir, curveType)) {
                boost::shared_ptr<Track> newTrack(new Track(existingTrack->refHexTile(), ptHexGrid::reverseDirection(existingTrack->calculateOutDir(firstExitCurveType))));
                newTrack->addExit(flipTrackCurve(firstExitCurveType));
                newTrack->addExit(flipTrackCurve(curveType));
                return newTrack;
            }
        }
        else if (existingTrack->numExits() == 2) {
            if (existingTrack->coInDir() == inDir) {
                existingTrack->addExit(curveType);
                return existingTrack;
            }
            else if (existingTrack->coInDir() == ptHexGrid::reverseDirection(calculateCurveOutDir(inDir, curveType))) {
                existingTrack->addExit(flipTrackCurve(curveType));
                return existingTrack;
            }
            return boost::shared_ptr<Track>();
        }
        else if (existingTrack->numExits() == 3) {
            return boost::shared_ptr<Track>();
        }
    }
    return boost::shared_ptr<Track>();
}

void tryLayTrack(TrackCurveType curveType) {
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

    boost::shared_ptr<Track> maybeNewTrack;

    // try to combine, if successful overwrite
    if (boost::shared_ptr<Track> existingTrack = currentTile->refTrack()) {
        boost::shared_ptr<Track> maybeCombinedTrack = tryCombineTracks(existingTrack, currentDir, curveType);
        if (maybeCombinedTrack) {
            maybeNewTrack = maybeCombinedTrack;
        }
    }
    else {
        maybeNewTrack = boost::shared_ptr<Track>(new Track(currentTile, currentDir, trackExits));
    }

    if (maybeNewTrack) {
        currentTile->setTrack(maybeNewTrack);

        // change currentTile pointer
        currentTile = gameMap.refTile(ptHexGrid::getNeighborInDirection(currentTile->coAxialPos(), nextTileDir));
        currentDir = nextTileDir;
    }
}

Cart::Cart(boost::shared_ptr<Track> _currentTrack, float _trackPosition, int _trackExit)
    : currentTrack(_currentTrack), trackPosition(_trackPosition), trackExit(_trackExit), speed(0)
    {}

boost::shared_ptr<Track> Cart::refTrack() {
    return currentTrack;
}

void Cart::go() {
    trackPosition += speed;
    if (trackPosition < 0 || trackPosition > 1) {
        // switch to next track and exit (or crash?)
            // which means finding next track...
        // negate speed if track is pointed opposite way
    }
}

int seed = 0;

int main (int argc, char **argv) {
    if (seed == 0) srand(time(NULL));
    else srand(seed);
    gameMap.generate();

    currentDir = ptHexGrid::Right;
    currentTile = gameMap.refTile(vector2i(1, 1));

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
                            tryLayTrack(TrackCurveType::Straight);
                            break;
                        case sf::Keyboard::Left:
                            tryLayTrack(TrackCurveType::CurveLeft);
                            break;
                        case sf::Keyboard::Right:
                            tryLayTrack(TrackCurveType::CurveRight);
                            break;
                        case sf::Keyboard::Space:
                            gameMap.generate();
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

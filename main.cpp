#include <SFML/Graphics.hpp>
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>
#include "myvectors.h"
#include "hexgrid.h"
#include <iostream>
#include <boost/shared_ptr.hpp>

using namespace std;

const float DEG2RAD = 3.14159/180;

void drawFTHex(float circumRadius) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0,0);
    for (int i=0; i<7; i++) {
        float rad = (i*(M_PI/3));
        glVertex2f(cos(rad)*circumRadius, -sin(rad)*circumRadius);
    }
    glEnd();
}

void drawFTHexBorder(float circumRadius) {
    glBegin(GL_LINE_LOOP);
    for (int i=0; i<6; i++) {
        float rad = (i*M_PI/3);
        glVertex2f(cos(rad) * circumRadius, -sin(rad) * circumRadius);
    }
    glEnd();
}

void drawPTHex(float circumRadius) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0,0);
    for (int i=0; i<7; i++) {
        float rad = (M_PI/6) + (i*(M_PI/3));
        glVertex2f(cos(rad)*circumRadius, -sin(rad)*circumRadius);
    }
    glEnd();
}

void drawPTHexBorder(float circumRadius) {
    glBegin(GL_LINE_LOOP);
    for (int i=0; i<6; i++) {
        float rad = (M_PI/6) + (i*M_PI/3);
        glVertex2f(cos(rad) * circumRadius, -sin(rad) * circumRadius);
    }
    glEnd();
}

class Map;
class HexTile;
class Track;

// class Track {
//     HexTile* hexTile;
//     ptHexGrid::Direction startDir;
//     bool exits[3]; // 0=left-curve, 1=straight, 2=right-curve
// public:
//     Track(HexTile* _hexTile, ptHexGrid::Direction _startDir, bool _exits[3]) {
//         hexTile = _hexTile;
//         startDir = _startDir;
//         for (int i=0; i<sizeof(exits); i++) {
//             exits[i] = _exits[i];
//         }
//     }
//     void draw() {
//         if (exits[0]) {
//             ptHexGrid::Direction towardNeighborForCircleCenter = dirTurnedCCW(startDir, 2);
//             vector2i neighborAxial = ptHexGrid::getNeighborInDirection(hexTile->coAxialPos(), towardNeighborForCircleCenter);
//             vector2f circleDrawCenter = hexTile->refMap()->refTile(neighborAxial)->coMapPos();
//             ptHexGrid::Direction towardArc = ptHexGrid::reverseDirection(towardNeighborForCircleCenter);
//             float arcStartAngle = ptHexGrid::directionToAngle(towardArc) - M_PI/6;
//             float circleDrawRadius = hexTile->refMap()->coTileCircumradius() * 1.1;

//             glTranslatef(circleDrawCenter.x, circleDrawCenter.y, 0);
//             glBegin(GL_LINES);
//             glColor3f(0,0,1);
//             for (int i=0; i <= RAIL_CURVE_NUM_POINTS; i++) {
//                 float angle = arcStartAngle + ((float(i) / RAIL_CURVE_NUM_POINTS) * (M_PI/3));
//                 glVertex2f(cos(angle) * circleDrawRadius, -sin(angle) * circleDrawRadius);
//             }
//             glEnd();
//         }
//     }
// };

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

const int MAX_MAP_DIM = 100;

class Map {
    float tileCircumradius;
    vector2i dimensions;
    HexTile tiles[MAX_MAP_DIM][MAX_MAP_DIM];

    // void setScale(float _tileCircumradius) {
    //     tileCircumradius = _tileCircumradius;
    // }
public:
    Map(float _tileCircumradius, int width, int height) {
        if ( width >= MAX_MAP_DIM || height >= MAX_MAP_DIM) {
            throw "Map dimensions exceed max";
        }

        dimensions = vector2i(width, height);
        tileCircumradius = _tileCircumradius;
        // cout << width << "," << height << endl;
        for (int q=0; q < width; q++) {
            for (int r=0; r < height; r++) {
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
    void generate() {

    }
    HexTile* refTile(vector2i axial) {
        if (axial.x >= dimensions.x || axial.y >= dimensions.y) {
            throw "axial pos out of map bounds";
        }
        return &(tiles[axial.x][axial.y]);
    }
    // use of 'coVar' convention to indicate copies (to help track mutability and side effects)
    float coTileCircumradius() { // 'copy of circumRadius'
        return tileCircumradius;
    }
    float coScale() { // helpful synonym
        return coTileCircumradius();
    }
    float coTileLongwidth() {
        return tileCircumradius * 2;
    }
    float coTileShortwidth() {
        return coTileLongwidth() * 3.0/4.0;
    }

    void drawHere() {
        for (int q=0; q < dimensions.x; q++) {
            for (int r=0; r < dimensions.y; r++) {
                tiles[q][r].drawAtPos();
            }
        }
    }
};

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
void HexTile::drawHere() {
    switch (tileType) {
        case TileType::Dirt:
            glColor3f(.6, .4, 0);
            break;
        case TileType::Grass:
            glColor3f(0,1,0);
            break;
        case TileType::Wall:
            glColor3f(0.4,0.4,0.4);
            break;
        case TileType::Water:
            glColor3f(0, 0, 1);
            break;
        default:
            throw "HexTile::drawHere: unrecognized tileType";
    }
    drawPTHex(map->coTileCircumradius());
    glColor3f(0,0,0);
    drawPTHexBorder(map->coTileCircumradius());
    
}
void HexTile::drawAtPos() {
    vector2f offset = coMapPos();
    // cout << offset.x << "," << offset.y <<  endl;
    glPushMatrix();
    glTranslatef(offset.x,offset.y,0);
    drawHere();
    glPopMatrix();
}

void glEnable2D() {
    GLint iViewport[4];

    //get a copy of the viewport
    glGetIntegerv(GL_VIEWPORT, iViewport);

    //save a copy of the projection matrix so we can restore it
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();

    //load identity projection matrix
    glLoadIdentity();

    //set up orthographic projection
    glOrtho(iViewport[0], iViewport[0]+iViewport[2], iViewport[1]+iViewport[3], iViewport[1], -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    //ensure lighting and depth testing are disabled
    glPushAttrib(GL_DEPTH_BUFFER_BIT | GL_LIGHTING_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
}

void glDisable2D() {
    glPopAttrib();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

// stupid expensive, reduce iterations before use!
// void drawCircle(float radius) {
//     glBegin(GL_TRIANGLE_FAN);
//     for (int i=0; i<360; i++) {
//         float rad = i*DEG2RAD;
//         glVertex2f(cos(rad)*radius, sin(rad)*radius);
//     }
//     glEnd();
// }

int main (int argc, char **argv) {
    Map map(32, 50, 50);
    map.generate();

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

                glClearColor(0,0,0,0);
                glClear(GL_COLOR_BUFFER_BIT);

                glEnable2D();
                glPushMatrix();
                glTranslatef(100,-500,0);
                map.drawHere();
                glPopMatrix();
                glDisable2D();

        window.display(); // Update window contents
    }
    return 0;
}

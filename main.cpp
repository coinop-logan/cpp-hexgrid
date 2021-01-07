#include <SFML/Graphics.hpp>
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>
#include "myvectors.h"
#include <iostream>

using namespace std;

const float DEG2RAD = 3.14159/180;

void drawFlatTopHex(float circumRadius) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0,0);
    for (int i=0; i<7; i++) {
        float rad = (i*(M_PI/3));
        glVertex2f(cos(rad)*circumRadius, -sin(rad)*circumRadius);
    }
    glEnd();
}

void drawFlatTopHexBorder(float circumRadius) {
    glBegin(GL_LINE_LOOP);
    for (int i=0; i<6; i++) {
        float rad = (i*M_PI/3);
        glVertex2f(cos(rad) * circumRadius, -sin(rad) * circumRadius);
    }
    glEnd();
}

class Map;

enum class TileType { Dirt, Grass, Wall, Water };

class HexTile {
    TileType tileType;
    vector2i axialPos;
    Map *map;
public:
    HexTile();
    HexTile(TileType _tileType, vector2i _axialPos, Map *_map);
    Map *refMap();
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
    }
    void generate() {
        for (int q=0; q < dimensions.x; q++) {
            for (int r=0; r < dimensions.y; r++) {
                // cout << q << "," << r << endl;
                TileType type = TileType::Dirt;

                // if ((q == 0 || r%4 == 0) && q !=5) {
                //     type = TileType::Wall;
                // }
                // if (r + q == 25) {
                //     type = TileType::Water;
                // }

                if (rand() % 20 == 0) {
                    type = TileType::Wall;
                }
                else if (q+r == 42) {
                    type = TileType::Water;
                }

                tiles[q][r] = HexTile(type, vector2i(q,r), this);
            }
        }
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

Map *HexTile::refMap() {
    return map;
}
vector2f HexTile::coMapPos() {
    return vector2f(
        map->coTileLongwidth() * (3./4 * axialPos.x),
        map->coTileLongwidth() * (sqrt(3)/4 * axialPos.x  +  sqrt(3)/2 * axialPos.y)
    );
}
void HexTile::drawHere() {
    switch (tileType) {
        case TileType::Dirt:
            glColor3f(1, 1, 1);
            break;
        case TileType::Grass:
            glColor3f(0,1,0);
            break;
        case TileType::Wall:
            glColor3f(0.4,0.4,0.4);
            break;
        case TileType::Water:
            glColor3f(0.7, 0.7, 0.7);
            break;
        default:
            throw "HexTile::drawHere: unrecognized tileType";
    }
    drawFlatTopHex(map->coTileCircumradius());
    glColor3f(0,0,0);
    drawFlatTopHexBorder(map->coTileCircumradius());
    
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

    // HexTile* viewTargetTile = &(map.getTile(vector2i(0,0)));

    // cout << hexTile1.refMap()->coTileLongwidth() << endl << endl;

    // cout << hexTile1.coMapPos().x << "," << hexTile1.coMapPos().y << endl;
    // cout << hexTile2.coMapPos().x << "," << hexTile2.coMapPos().y << endl;
    // cout << hexTile3.coMapPos().x << "," << hexTile3.coMapPos().y << endl;

    sf::RenderWindow window(sf::VideoMode(1920, 1080), "HexStuff", sf::Style::Fullscreen);
    // unfortunately renders on the non-streaming window
    // sf::RenderWindow window(sf::VideoMode(1800, 950), "HexStuff", sf::Style::Close | sf::Style::Titlebar);

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
                glTranslatef(0,-1200,0);
                map.drawHere();
                glPopMatrix();
                glDisable2D();

        window.display(); // Update window contents
    }
    return 0;
}

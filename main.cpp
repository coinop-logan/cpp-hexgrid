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

class Map {
    float tileCircumradius;
public:
    Map(float _tileCircumradius) {
        tileCircumradius = _tileCircumradius;
    }
    void setScale(float _tileCircumradius) {
        tileCircumradius = _tileCircumradius;
    }
    // use of 'coVar' convention to indicate copies (to help track mutability and side effects)
    float coScale() { // 'copy of Scale'
        return tileCircumradius;
    }
    float coTileCircumradius() { // helpful synonym
        return coScale();
    }
    float coTileLongwidth() {
        return tileCircumradius * 2;
    }
    float coTileShortwidth() {
        return coTileLongwidth() * 3.0/4.0;
    }
};

enum class TileType { Dirt, Grass, Wall } ;

class HexTile {
    TileType tileType;
    vector2i axialPos;
    Map *map;
    ;
public:
    HexTile(TileType _tileType, vector2i _axialPos, Map *_map) {
        tileType = _tileType;
        axialPos = _axialPos;
        map = _map;
    }
    Map *refMap() {
        return map;
    }
    vector2f coMapPos() {
        return vector2f(
            map->coTileLongwidth() * (3./4 * axialPos.x),
            map->coTileLongwidth() * (sqrt(3)/4 * axialPos.x  +  sqrt(3)/2 * axialPos.y)
        );
    }
    void drawHere() {
        switch (tileType) {
            case TileType::Dirt:
                glColor3f(.8, .6, 0);
                break;
            case TileType::Grass:
                glColor3f(0,1,0);
                break;
            case TileType::Wall:
                glColor3f(0.4,0.4,0.4);
                break;
        }
        drawFlatTopHex(map->coTileCircumradius());
        
    }
    void drawAtPos() {
        vector2f offset = coMapPos();
        glPushMatrix();
        glTranslatef(offset.x,offset.y,0);
        drawHere();
        glPopMatrix();
    }
};

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
    Map map(16);

    HexTile hexTile1(TileType::Dirt, vector2i(0,0), &map);
    HexTile hexTile2(TileType::Grass, vector2i(1,0), &map);
    HexTile hexTile3(TileType::Wall, vector2i(0,1), &map);

    cout << hexTile1.refMap()->coTileLongwidth() << endl << endl;

    cout << hexTile1.coMapPos().x << "," << hexTile1.coMapPos().y << endl;
    cout << hexTile2.coMapPos().x << "," << hexTile2.coMapPos().y << endl;
    cout << hexTile3.coMapPos().x << "," << hexTile3.coMapPos().y << endl;

    sf::RenderWindow window(sf::VideoMode(640, 480), "OpenGL Test", sf::Style::Close | sf::Style::Titlebar);

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
                glTranslatef(100,100,0);
                hexTile1.drawAtPos();
                hexTile2.drawAtPos();
                hexTile3.drawAtPos();
                glPopMatrix();
                glDisable2D();

        window.display(); // Update window contents
    }
    return 0;
}

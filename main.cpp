#include <SFML/Graphics.hpp>
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>
#include "myvectors.h"

class HexTile {
    vector2i axial;
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

const float DEG2RAD = 3.14159/180;

void drawCircle(float radius) {
    glBegin(GL_TRIANGLE_FAN);
    for (int i=0; i<360; i++) {
        float rad = i*DEG2RAD;
        glVertex2f(cos(rad)*radius, sin(rad)*radius);
    }
    glEnd();
}

float x = 50;

int main (int argc, char **argv) {
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
                    x += 1;
                    break;
            }
        }
        window.clear(); // Clear the buffer

                glClearColor(0,0,0,0);
                glClear(GL_COLOR_BUFFER_BIT);
                glColor3f(0,1,0);

                glEnable2D();
                glPushMatrix();
                glTranslatef(x,50,0);
                drawCircle(5);
                glPopMatrix();
                glDisable2D();

        window.display(); // Update window contents
    }
    return 0;
}

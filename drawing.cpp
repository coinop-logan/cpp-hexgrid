#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "drawing.h"
#include "entities.h"

void glVertex2f(vector2f vec) {
    glVertex2f(vec.x, -vec.y);
}

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

void drawCurvedTrack(ptHexGrid::Direction startDir, bool left, float circumradius) {
    // find the center of the circle to draw the arc from - a nearby tile neighbor.
    ptHexGrid::Direction towardNeighborForCircleCenter = dirTurned(startDir, left ? -2 : 2);
    vector2i circleCenterAxial = ptHexGrid::directionToAxial(towardNeighborForCircleCenter);
    vector2f circleCenter = ptHexGrid::axialToReal(circleCenterAxial, circumradius);

    // define some radii for the arcs to draw
    float trackCenterArcRadius = circumradius * 1.5;
    float trackInsideArcRadius = trackCenterArcRadius - circumradius * TRACK_DRAW_HALFWIDTH_OVER_CIRCUMRADIUS;
    float trackOutsideArcRadius = trackCenterArcRadius + circumradius * TRACK_DRAW_HALFWIDTH_OVER_CIRCUMRADIUS;

    ptHexGrid::Direction towardArc = ptHexGrid::reverseDirection(towardNeighborForCircleCenter);
    float arcStartAngle = ptHexGrid::directionToAngle(towardArc) - M_PI/6;

    glPushMatrix();
        glTranslatef(circleCenter.x, circleCenter.y, 0);
        glBegin(GL_LINE_STRIP);
            glColor3f(0,0,1);
            for (int i=0; i <= RAIL_CURVE_NUM_POINTS; i++) {
                float angle = arcStartAngle + ((float(i) / RAIL_CURVE_NUM_POINTS) * (M_PI/3));
                glVertex2f(cos(angle) * trackInsideArcRadius, -sin(angle) * trackInsideArcRadius);
            }
        glEnd();
        glBegin(GL_LINE_STRIP);
            glColor3f(0,0,1);
            for (int i=0; i <= RAIL_CURVE_NUM_POINTS; i++) {
                float angle = arcStartAngle + ((float(i) / RAIL_CURVE_NUM_POINTS) * (M_PI/3));
                glVertex2f(cos(angle) * trackOutsideArcRadius, -sin(angle) * trackOutsideArcRadius);
            }
        glEnd();
    glPopMatrix();
}

void drawStraightTrack(ptHexGrid::Direction dir, float circumradius) {
    float inradius = ptHexGrid::getInradius(circumradius);
    float trackAngle = ptHexGrid::directionToAngle(dir);
    float perpAngle = trackAngle + M_PI / 2;

    vector2f trackEndCenter = composeVector2f(trackAngle, inradius);
    vector2f trackStartCenter = trackEndCenter * -1;

    vector2f trackSideOffset = composeVector2f(perpAngle, circumradius * TRACK_DRAW_HALFWIDTH_OVER_CIRCUMRADIUS);

    glBegin(GL_LINES);
        glColor3f(0,0,1);
        glVertex2f(trackStartCenter + trackSideOffset);
        glVertex2f(trackEndCenter + trackSideOffset);
        glVertex2f(trackStartCenter - trackSideOffset);
        glVertex2f(trackEndCenter - trackSideOffset);
    glEnd(); 
}

void Track::drawHere() {
    float circumradius = hexTile->refMap()->coTileCircumradius();
    if (exits[0]) {
        drawCurvedTrack(
            startDir,
            true,
            circumradius
        );
    }
    if (exits[1]) {
        drawStraightTrack(startDir, circumradius);
    }
    if (exits[2]) {
        drawCurvedTrack(
            startDir,
            false,
            circumradius
        );
    }
}

void GameMap::drawHere() {
    for (int q=0; q < dimensions.x; q++) {
        for (int r=0; r < dimensions.y; r++) {
            tiles[q][r].drawAtPos();
        }
    }
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

    if (track) {
        track->drawHere();
    }
    
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
void draw(GameMap *map) {
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable2D();
    glPushMatrix();
    glTranslatef(100,-500,0);
    map->drawHere();
    glPopMatrix();
    glDisable2D();
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
#include <myvectors.h>

#ifndef HEXGRID_H
#define HEXGRID_H

namespace ptHexGrid {
    enum Direction {
        Right, UpRight, UpLeft, Left, DownLeft, DownRight
    };

    float directionToAngle(Direction dir);
    vector2i directionToAxial(Direction dir);
    Direction dirTurnedCCWOnce(Direction dir);
    Direction dirTurnedCWOnce(Direction dir);
    Direction dirTurnedCCW(Direction dir, int numTimes);
    Direction dirTurnedCW(Direction dir, int numTimes);
    Direction dirTurned(Direction dir, int signedNumTimes);
    Direction reverseDirection(Direction dir);
    vector2i getNeighborInDirection(vector2i startAxial, Direction dir);
    const vector2f qBasis(sqrt(3), 0);
    const vector2f rBasis(sqrt(3)/2.0, 3/2.0);
    vector2f axialToReal(vector2i axial, float circumRadius);
}

namespace ftHexGrid {
    enum Direction {
        RightUp, Up, LeftUp, LeftDown, Down, RightDown
    };

    Direction dirTurnedCCWOnce(Direction dir);
    Direction dirTurnedCWOnce(Direction dir) ;
    Direction dirTurnedCCW(Direction dir, int numTimes);
    Direction dirTurnedCW(Direction dir, int numTimes);
    Direction dirTurned(Direction dir, int signedNumTimes);
    vector2i getNeighborInDirection(vector2i startAxial, Direction dir);
}

#endif
#include <myvectors.h>
#include "hexgrid.h"

namespace ptHexGrid {
    float getInradius(float circumradius) {
        return circumradius * (sqrt(3) / 2);
    }
    float directionToAngle(Direction dir) {
        switch (dir) {
            case Right:
                return 0;
                break;
            case UpRight:
                return (M_PI/3.0) * 1;
                break;
            case UpLeft:
                return (M_PI/3.0) * 2;
                break;
            case Left:
                return (M_PI/3.0) * 3;
                break;
            case DownLeft:
                return (M_PI/3.0) * 4;
                break;
            case DownRight:
                return (M_PI/3.0) * 5;
                break;
        }
    }
    vector2i directionToAxial(Direction dir) {
        switch (dir) {
            case Right:
                return vector2i(1, 0);
                break;
            case UpRight:
                return vector2i(1, -1);
                break;
            case UpLeft:
                return vector2i(0, -1);
                break;
            case Left:
                return vector2i(-1, 0);
                break;
            case DownLeft:
                return vector2i(-1, 1);
                break;
            case DownRight:
                return vector2i(0, 1);
                break;
        }
    }

    Direction dirTurnedCCWOnce(Direction dir) {
        switch (dir) {
            case Right:
                return UpRight;
                break;
            case UpRight:
                return UpLeft;
                break;
            case UpLeft:
                return Left;
                break;
            case Left:
                return DownLeft;
                break;
            case DownLeft:
                return DownRight;
                break;
            case DownRight:
                return Right;
                break;
        }
    }
    Direction dirTurnedCWOnce(Direction dir) {
        switch (dir) {
            case UpRight:
                return Right;
                break;
            case UpLeft:
                return UpRight;
                break;
            case Left:
                return UpLeft;
                break;
            case DownLeft:
                return Left;
                break;
            case DownRight:
                return DownLeft;
                break;
            case Right:
                return DownRight;
                break;
        }
    }
    Direction dirTurnedCCW(Direction dir, int numTimes) {
        for (int i=0; i<numTimes; i++) {
            dir = dirTurnedCCWOnce(dir);
        }
        return dir;
    }
    Direction dirTurnedCW(Direction dir, int numTimes) {
        for (int i=0; i<numTimes; i++) {
            dir = dirTurnedCWOnce(dir);
        }
        return dir;
    }
    Direction dirTurned(Direction dir, int signedNumTimes) {

        if (signedNumTimes > 0) {
            return dirTurnedCW(dir, signedNumTimes);
        }
        else {
            return dirTurnedCCW(dir, -signedNumTimes);
        }
    }
    Direction reverseDirection(Direction dir) {
        switch (dir) {
            case UpRight:
                return DownLeft;
                break;
            case UpLeft:
                return DownRight;
                break;
            case Left:
                return Right;
                break;
            case DownLeft:
                return UpRight;
                break;
            case DownRight:
                return UpLeft;
                break;
            case Right:
                return Left;
                break;
        }
    }

    vector2i getNeighborInDirection(vector2i startAxial, Direction dir) {
        return startAxial + directionToAxial(dir);
    }

    vector2f axialToReal(vector2i axial, float circumRadius) {
        return (vector2f(qBasis) * axial.x + vector2f(rBasis) * axial.y) * circumRadius;
    }
}

namespace ftHexGrid {
    Direction dirTurnedCCWOnce(Direction dir) {
        switch (dir) {
            case RightUp:
                return Up;
                break;
            case Up:
                return LeftUp;
                break;
            case LeftUp:
                return LeftDown;
                break;
            case LeftDown:
                return Down;
                break;
            case Down:
                return RightDown;
                break;
            case RightDown:
                return RightUp;
                break;
        }
    }

    Direction dirTurnedCWOnce(Direction dir) {
        switch (dir) {
            case RightUp:
                return RightDown;
                break;
            case Up:
                return RightUp;
                break;
            case LeftUp:
                return Up;
                break;
            case LeftDown:
                return LeftUp;
                break;
            case Down:
                return LeftDown;
                break;
            case RightDown:
                return Down;
                break;
        }
    }

    Direction dirTurnedCCW(Direction dir, int numTimes) {
        for (int i=0; i<numTimes; i++) {
            dir = dirTurnedCCWOnce(dir);
        }
        return dir;
    }

    Direction dirTurnedCW(Direction dir, int numTimes) {
        for (int i=0; i<numTimes; i++) {
            dir = dirTurnedCWOnce(dir);
        }
        return dir;
    }

    Direction dirTurned(Direction dir, int signedNumTimes) {
        if (signedNumTimes > 0) {
            return dirTurnedCW(dir, signedNumTimes);
        }
        else {
            return dirTurnedCCW(dir, -signedNumTimes);
        }
    }

    vector2i getNeighborInDirection(vector2i startAxial, Direction dir) {
        vector2i offset;
        switch (dir) {
            case RightUp:
                offset = vector2i(1, -1);
                break;
            case Up:
                offset = vector2i(0, -1);
                break;
            case LeftUp:
                offset = vector2i(-1, 0);
                break;
            case LeftDown:
                offset = vector2i(-1, 1);
                break;
            case Down:
                offset = vector2i(0, 1);
                break;
            case RightDown:
                offset = vector2i(1, 0);
                break;
        }
        return startAxial + offset;
    }
}
#ifndef DRAWING_H
#define DRAWING_H

#include "entities.h"

void drawFTHex(float circumRadius);
void drawFTHexBorder(float circumRadius);
void drawPTHex(float circumRadius);
void drawPTHexBorder(float circumRadius);

void glEnable2D();
void glDisable2D();

void draw(Map *map);

#endif
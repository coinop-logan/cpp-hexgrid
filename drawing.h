#ifndef DRAWING_H
#define DRAWING_H

#include "myvectors.h"
#include "entities.h"

void glVertex2f(vector2f);

void drawFTHex(float circumRadius);
void drawFTHexBorder(float circumRadius);
void drawPTHex(float circumRadius);
void drawPTHexBorder(float circumRadius);

void glEnable2D();
void glDisable2D();

void draw(GameMap *map);

#endif
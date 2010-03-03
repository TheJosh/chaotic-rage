#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


#define PI 3.1415

#define MAX(a,b) ((a) >= (b) ? (a) : (b))
#define MIN(a,b) ((a) <= (b) ? (a) : (b))

// Trig
int getAngleBetweenPoints (int point1_x, int point1_y, int point2_x, int point2_y);
int pointPlusAngleX (int point_x, int angle, int distance);
int pointPlusAngleY (int point_y, int angle, int distance);

// Boxes and stuff
int intersect (SDL_Rect one, SDL_Rect two);
int inside (SDL_Rect rect, int x, int y);
int collideWall(GameState *st, int x, int y, int w, int h);

// Time
int ppsDelta(int pps, int delta);

// Random numbers
void seedRandom();
int getRandom(int low, int high);

// libconfuse helpers
Range cfg_getrange(cfg_t *cfg, const char * name);

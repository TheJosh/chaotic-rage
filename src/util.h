// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


#define PI 3.1415

#define MAX(a,b) ((a) >= (b) ? (a) : (b))
#define MIN(a,b) ((a) <= (b) ? (a) : (b))

#define DF_AREATYPES 1
#define DF_UNITCLASS 2
#define DF_PARTICLES 3
#define DF_WEAPONS 4

// Trig
int getAngleBetweenPoints (int point1_x, int point1_y, int point2_x, int point2_y);
int clampAngle(int angle);
int angleFromDesired(int current, int desired, int turn_speed);
float pointPlusAngleX (float point_x, int angle, float distance);
float pointPlusAngleY (float point_y, int angle, float distance);

// Boxes and stuff
int intersect (SDL_Rect one, SDL_Rect two);
int inside (SDL_Rect rect, int x, int y);

// Time
int ppsDeltai(int pps, int delta);
float ppsDeltaf(float pps, int delta);

// Random numbers
void seedRandom();
int getRandom(int low, int high);

// libconfuse helpers
Range cfg_getrange(cfg_t *cfg, const char * name);

// Game mods
string getDataDirectory(int datafile);

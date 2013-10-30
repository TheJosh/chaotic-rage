// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include "../rage.h"
#include <glm/glm.hpp>
#include <btBulletDynamicsCommon.h>

using namespace std;


#define PI 3.1415f

#define MAX(a,b) ((a) >= (b) ? (a) : (b))
#define MIN(a,b) ((a) <= (b) ? (a) : (b))

#define DEG_TO_RAD(x) ((x) * (PI / 180.0f))
#define RAD_TO_DEG(x) ((x) * (180.0f / PI))


// Trig
int getAngleBetweenPoints (int point1_x, int point1_y, int point2_x, int point2_y);
float pointPlusAngleX (float point_x, int angle, float distance);
float pointPlusAngleY (float point_y, int angle, float distance);

// Rotation clamping
int clampAngle(int angle);
float clampAnglef(float angle);
int angleFromDesired(int current, int desired, int turn_speed);

// Random numbers
void seedRandom();
int getRandom(int low, int high);
float getRandomf(float low, float high);

// conversion
void glmBullet(glm::mat4 in, btMatrix3x3 & out);
// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include <math.h>
#include <time.h>
#include <confuse.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <btBulletDynamicsCommon.h>
#include "../rage.h"

using namespace std;


/**
* Returns an angle, in degrees, between two points
**/
int getAngleBetweenPoints (int point1_x, int point1_y, int point2_x, int point2_y)
{
	float angle;
	
	if ((point1_x - point2_x) == 0) {
		if (point1_y > point2_y) {
			angle = 0.0f;
		} else {
			angle = PI;
		}
		
	} else {
		angle = atan((float)(point1_y - point2_y) / (float)(point1_x - point2_x));
		if (point1_x > point2_x) {
			angle = PI / 2.0f - angle;
		} else {
			angle = PI * 1.5f - angle;
		}
	}
	
	angle = angle * (180.0f / PI);
	
	return (int) round(angle);
}


/**
* Ensures angles are in the range 0 - 359
**/
int clampAngle(int angle)
{
	while (angle < 0) angle += 360;
	while (angle > 359) angle -= 360;
	return angle;
}


/**
* Ensures angles are in the range 0 - 359
**/
float clampAnglef(float angle)
{
	while (angle < 0.0f) angle += 360.0f;
	while (angle > 360.0f) angle -= 360.0f;
	return angle;
}


/**
* Calculates a new angle based on the original angle, desired angle and turn speed
*
* @param int current The current angle, degrees
* @param int desired The desired angle, degrees
* @param int turn_speed The max speed of the turn, in degrees
**/
int angleFromDesired(int current, int desired, int turn_speed)
{
	// Calculate clockwise and anti-clockwise difference
	int diff_anti = desired;
	
	if (diff_anti < current) diff_anti += 360;
	diff_anti -= current;
	
	int diff_clock = 360 - diff_anti;
	
	// the shortest distance - go in that direction
	if (diff_anti < diff_clock) {
		current += (turn_speed < diff_anti ? turn_speed : diff_anti);
	} else {
		current -= (turn_speed < diff_clock ? turn_speed : diff_clock);
	}
	
	// clamp to legal values
	if (current < 0) current += 360;
	if (current > 359) current -= 360;
	
	return current;
}


/**
* Returns a new X co-ord for a point that has had angle and distance added to it
* Angle is specified in degrees
**/
float pointPlusAngleX (float point_x, int angle, float distance)
{
	float angle_rads;
	
	if (angle < 0) angle += 360;
	if (angle > 360) angle -= 360;
	
	angle_rads = angle * (PI / 180.0f);
	
	return point_x - (sin(angle_rads) * distance);
}


/**
* Returns a new Y co-ord for a point that has had angle and distance added to it
* Angle is specified in degrees
**/
float pointPlusAngleY (float point_y, int angle, float distance)
{
	float angle_rads;
	
	if (angle < 0) angle += 360;
	if (angle > 360) angle -= 360;
	
	angle_rads = angle * (PI / 180.0f);
	
	return point_y - (cos(angle_rads) * distance);
}



void seedRandom()
{
	srand(time(NULL));
}

int getRandom(int low, int high)
{
	if (high < low) swap(low, high);
	if (high == low) return low;
	return rand() % (high - low + 1) + low;
}

float getRandomf(float low, float high)
{
	if (high < low) swap(low, high);
	if (high == low) return low;
	return ((float)rand()/(float)RAND_MAX) * (high - low) + low;
}


/**
* Convert GLM to Bullet (matrix)
**/
void glmBullet(glm::mat4 in, btMatrix3x3 & out)
{
	out.setFromOpenGLSubMatrix(glm::value_ptr(in));
}


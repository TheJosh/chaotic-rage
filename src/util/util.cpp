// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include <math.h>
#include <time.h>
#include <confuse.h>
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
			angle = 0.0;
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
	
	angle = angle * (180 / PI);
	
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
	
	angle_rads = angle * (PI / 180.0);
	
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
	
	angle_rads = angle * (PI / 180.0);
	
	return point_y - (cos(angle_rads) * distance);
}


/**
* Returns 1 if the two rectangles intersect, else returns 0
**/
int intersect (SDL_Rect one, SDL_Rect two)
{
	if (
		(one.x > (two.x + two.w)) ||
		((one.x + one.w) < two.x) ||
		(one.y > (two.y + two.h)) ||
		((one.y + one.h) < two.y)
	) {
		return 0;
	}
	
	return 1;
}


/**
* Returns 1 if a point is inside a rectangle, else returns 0
**/
int inside (SDL_Rect rect, int x, int y)
{
	if (
		(x > rect.x) &&
		(y > rect.y) &&
		(x < rect.x + rect.w) &&
		(y < rect.y + rect.h)
	) {
		return 1;
	}
	
	return 0;
}


/**
* Converts a pps distance, and a delta into a fixed pixel distance.
* INTEGER version of function, for speed, position, etc.
*
* pps is 'pixels per second'.
* deltas are in miliseconds.
* return value is in pixels.
**/
int ppsDeltai(int pps, int delta)
{
	float ret = ((float)pps) * ((float)delta) / 1000.0;
	if (pps > 0) { return (int) ceil(ret); } else { return (int) floor(ret); }
}

/**
* Converts a pps distance, and a delta into a fixed pixel distance.
* FLOAT version of function, for angles, generators, etc.
*
* pps is 'pixels per second'.
* deltas are in miliseconds.
* return value is in pixels.
**/
float ppsDeltaf(float pps, int delta)
{
	return pps * ((float)delta) / 1000.0;
}


void seedRandom()
{
	srand (time(NULL));
}

int getRandom(int low, int high)
{
	if (high < low) swap(low, high);
	if (high == low) return low;
	return rand() % (high - low + 1) + low;
}


/**
* Gets a range for a libconfuse config option.
* A range is a min-max value pair in a list
**/
Range cfg_getrange(cfg_t *cfg, const char * name)
{
	Range res;
	
	if (cfg_size(cfg, name) == 1) {
		res.min = cfg_getnint(cfg, name, 0);
		res.max = res.min;
		
	} else {
		res.min = cfg_getnint(cfg, name, 0);
		res.max = cfg_getnint(cfg, name, 1);
	}
	
	return res;
}


/**
* Gets the name of the directory to load data from
* Ends in trailing slash.
* Datafile should be one of the DF_ constants in util.h
**/
string getDataDirectory(int datafile)
{
	switch (datafile) {
		case DF_AREATYPES:
			return "areatypes/";
			
		case DF_UNITCLASS:
			return "unitclass/";
		
		case DF_PARTICLES:
			return "particletypes/";
			
		case DF_WEAPONS:
			return "weapontypes/";
	}
	
	cerr << "Cannot find data directory for type = " << datafile << "\n";
	exit(1);
	
	return "";
}


void terminal_clear()
{
	printf("%c[2J\n", 0x1B);
}


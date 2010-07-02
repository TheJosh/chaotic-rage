#include <iostream>
#include <SDL.h>
#include <math.h>
#include <time.h>
#include <confuse.h>
#include "rage.h"

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
	
	return round(angle);
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
int pointPlusAngleX (int point_x, int angle, int distance)
{
	float angle_rads;
	float point;
	
	if (angle < 0) angle += 360;
	if (angle > 360) angle -= 360;
	
	angle_rads = angle * (PI / 180.0);
	
	point = point_x;
	point -= round(sin(angle_rads) * distance);
	
	return round(point);
}


/**
* Returns a new Y co-ord for a point that has had angle and distance added to it
* Angle is specified in degrees
**/
int pointPlusAngleY (int point_y, int angle, int distance)
{
	float angle_rads;
	float point;
	
	if (angle < 0) angle += 360;
	if (angle > 360) angle -= 360;
	
	angle_rads = angle * (PI / 180.0);
	
	point = point_y;
	point -= round(cos(angle_rads) * distance);
	
	return point;
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
* Returns 1 if any of the points in the rectangle collides with a wall
**/
int collideWall(GameState *st, int x, int y, int w, int h)
{
	AreaType *at;
	
	at = getAreaTypeByID(st->map->getDataAt(x, y).type);
	if (at->wall) return 1;
	
	at = getAreaTypeByID(st->map->getDataAt(x + w, y).type);
	if (at->wall) return 1;
	
	at = getAreaTypeByID(st->map->getDataAt(x, y + h).type);
	if (at->wall) return 1;
	
	at = getAreaTypeByID(st->map->getDataAt(x + w, y + h).type);
	if (at->wall) return 1;
	
	return 0;
}


/**
* Converts a pps distance, and a delta into a fixed pixel distance.
*
* pps is 'pixels per second'.
* deltas are in miliseconds.
* return value is in pixels.
**/
int ppsDelta(int pps, int delta)
{
	return ceil(((float)pps) * ((float)delta) / 1000.0);
}


void seedRandom()
{
	srand (time(NULL));
}

int getRandom(int low, int high)
{
	if (high < low) swap(low, high);
	if (high == low) return low;
	return rand() % (high-low) + low;
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
			return "data/cr/areatypes/";
			
		case DF_UNITCLASS:
			return "data/cr/unitclass/";
		
		case DF_PARTICLES:
			return "data/cr/particletypes/";
			
		case DF_WEAPONS:
			return "data/cr/weapontypes/";
	}
	
	cerr << "Cannot find data directory for type = " << datafile << "\n";
	exit(1);
	
	return "";
}


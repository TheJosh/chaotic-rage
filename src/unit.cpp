#include <iostream>
#include <SDL.h>
#include <math.h>
#include "rage.h"

using namespace std;


Unit::Unit(UnitClass *unit_class)
{
	this->uc = unit_class;
	this->angle = 0;
	this->speed = 0;
	this->health = 100;
}

Unit::~Unit()
{
}

void Unit::update(int delta, UnitClassSettings *ucs)
{
	int turn_speed = ppsDelta(ucs->turn_speed, delta);
	
	// Calculate clockwise and anti-clockwise difference
	int diff_anti = this->desired_angle;
	if (diff_anti < this->angle) diff_anti += 360;
	diff_anti -= this->angle;
	int diff_clock = 360 - diff_anti;
	
	// the shortest distance - go in that direction
	if (diff_anti < diff_clock) {
		this->angle += (turn_speed < diff_anti ? turn_speed : diff_anti);
	} else if (diff_clock < diff_anti) {
		this->angle -= (turn_speed < diff_clock ? turn_speed : diff_clock);
	}
	
	// clamp to legal values
	if (this->angle < 0) this->angle += 360;
	if (this->angle > 360) this->angle -= 360;
	
	
	this->x = pointPlusAngleX(this->x, this->angle, ppsDelta(this->speed, delta));
	this->y = pointPlusAngleY(this->y, this->angle, ppsDelta(this->speed, delta));
}



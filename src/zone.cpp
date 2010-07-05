// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;



Zone::Zone(int point1_x, int point1_y, int point2_x, int point2_y)
{
	this->x1 = point1_x;
	this->y1 = point1_y;
	this->x2 = point2_x;
	this->y2 = point2_y;
	
	for (int i = 0; i < NUM_FACTIONS; i++) this->spawn[i] = 0;
	for (int i = 0; i < NUM_FACTIONS; i++) this->prison[i] = 0;
	for (int i = 0; i < NUM_FACTIONS; i++) this->collect[i] = 0;
	for (int i = 0; i < NUM_FACTIONS; i++) this->dest[i] = 0;
	for (int i = 0; i < NUM_FACTIONS; i++) this->nearbase[i] = 0;
}

Zone::~Zone()
{
}


int Zone::getRandomX()
{
	return getRandom(this->x1, this->x2);
}

int Zone::getRandomY()
{
	return getRandom(this->y1, this->y2);
}

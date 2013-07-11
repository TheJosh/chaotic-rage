// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include "rage.h"
#include "zone.h"

using namespace std;



Zone::Zone(int x, int y, int width, int height)
{
	this->x1 = x - width / 2;
	this->y1 = y - height / 2;
	this->x2 = x + width / 2;
	this->y2 = y + height / 2;
	
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

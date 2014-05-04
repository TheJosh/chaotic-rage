// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include "../rage.h"
#include "zone.h"

using namespace std;



Zone::Zone(float x, float y, float width, float height)
{
	this->x1 = x - width / 2.0f;
	this->y1 = y - height / 2.0f;
	this->x2 = x + width / 2.0f;
	this->y2 = y + height / 2.0f;
	
	for (int i = 0; i < NUM_FACTIONS; i++) {
		this->spawn[i] = 0;
		this->prison[i] = 0;
		this->collect[i] = 0;
		this->dest[i] = 0;
		this->nearbase[i] = 0;
	}
}

Zone::~Zone()
{
}


float Zone::getRandomX()
{
	return getRandomf(this->x1, this->x2);
}

float Zone::getRandomY()
{
	return getRandomf(this->y1, this->y2);
}

// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <algorithm>
#include <math.h>
#include "../rage.h"
#include "obj.h"

using namespace std;


WavefrontObj::WavefrontObj()
{
	this->vao = NULL;
	this->count = 0;
}


/**
* Calculates the size of the bounding box for a given mesh
**/
void WavefrontObj::calcBoundingSize()
{
	float x1, x2, y1, y2, z1, z2;

	x1 = x2 = y1 = y2 = z1 = z2 = 0.0f;

	for (unsigned int i = 0; i < this->vertexes.size(); i++) {
		x1 = MIN(x1, this->vertexes[i].x);
		x2 = MAX(x2, this->vertexes[i].x);

		y1 = MIN(y1, this->vertexes[i].y);
		y2 = MAX(y2, this->vertexes[i].y);

		z1 = MIN(z1, this->vertexes[i].z);
		z2 = MAX(z2, this->vertexes[i].z);
	}

	this->size.x = x2 - x1;
	this->size.y = y2 - y1;
	this->size.z = z2 - z1;


}

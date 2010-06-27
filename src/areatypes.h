#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


class AreaType
{
	public:
		// from data file
		bool stretch;
		bool wall;
		
		// dynamic
		int id;
		SpritePtr surf;
		
	public:
		AreaType();
};


bool loadAllAreaTypes(Render * render);
AreaType* getAreaTypeByID(unsigned int id);

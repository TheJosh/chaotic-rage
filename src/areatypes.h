#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


class AreaType
{
	public:
		// from data file
		string tex;
		bool stretch;
		bool wall;
		
		// dynamic
		int id;
		SDL_Surface *surf;
		
	public:
		AreaType();
};


bool loadAllAreaTypes();
AreaType* getAreaTypeByID(unsigned int id);

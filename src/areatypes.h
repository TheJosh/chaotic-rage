#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


class AreaType
{
	public:
		int id;
		string tex;
		bool stretch;
		
		SDL_Surface *surf;
		
	public:
		AreaType();
};


bool loadAllAreaTypes();
AreaType* getAreaTypeByID(int id);

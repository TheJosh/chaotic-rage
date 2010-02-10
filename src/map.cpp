#include <iostream>
#include <SDL.h>
#include <math.h>
#include "rage.h"

using namespace std;

/**
* Load a file (simulated)
**/
int Map::load(string name)
{
	Area *a;
	
	this->width = 1000;
	this->height = 1000;
	
	a = new Area();
	a->x = 10;
	a->y = 10;
	a->width = 200;
	a->height = 100;
	a->type = getAreaTypeByID(1);
	this->areas.push_back(a);
	
	a = new Area();
	a->x = 50;
	a->y = 170;
	a->width = 50;
	a->height = 300;
	a->type = getAreaTypeByID(1);
	this->areas.push_back(a);
	
	return 1;
}


/**
* Render a single frame of the wall animation
**/
SDL_Surface* Map::renderWallFrame(int frame)
{
	SDL_Surface* surf = SDL_CreateRGBSurface(SDL_SWSURFACE, this->width, this->height, 32, 0,0,0,0);
	
	Area *a;
	unsigned int i;
	SDL_Rect src;
	SDL_Rect dest;
	
	for (i = 0; i < this->areas.size(); i++) {
		a = this->areas[i];
		
		src.x = 0;
		src.y = 0;
		src.w = a->width;
		src.h = a->height;
		
		dest.x = a->x;
		dest.y = a->y;
		
		SDL_BlitSurface(a->type->surf, &src, surf, &dest);
	}
	
	return surf;
}


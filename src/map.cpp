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
	
	// Default area
	a = new Area();
	a->x = 0;
	a->y = 0;
	a->width = this->width;
	a->height = this->height;
	a->angle = 0;
	a->type = getAreaTypeByID(0);
	this->areas.push_back(a);
	
	a = new Area();
	a->x = 300;
	a->y = 300;
	a->width = 100;
	a->height = 100;
	a->angle = 22;
	a->type = getAreaTypeByID(2);
	this->areas.push_back(a);
	
	a = new Area();
	a->x = 150;
	a->y = 170;
	a->width = 100;
	a->height = 100;
	a->angle = 0;
	a->type = getAreaTypeByID(2);
	this->areas.push_back(a);
	
	a = new Area();
	a->x = 0;
	a->y = 0;
	a->width = 200;
	a->height = 1000;
	a->angle = 2;
	a->type = getAreaTypeByID(3);
	this->areas.push_back(a);
	
	a = new Area();
	a->x = 400;
	a->y = 30;
	a->width = 200;
	a->height = 30;
	a->angle = 0;
	a->type = getAreaTypeByID(3);
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
	//SDL_Rect src;
	SDL_Rect dest;
	
	for (i = 0; i < this->areas.size(); i++) {
		a = this->areas[i];
		
		SDL_Surface *areasurf = a->type->surf;
		
		if (a->type->stretch)  {
			areasurf = rotozoomSurfaceXY(areasurf, 0, ((double)a->width) / ((double)areasurf->w), ((double)a->height) / ((double)areasurf->h), 0);
			if (areasurf == NULL) continue;
			
		} else {
			areasurf = tileSprite(areasurf, a->width, a->height);
			if (areasurf == NULL) continue;
		}
		
		if (a->angle != 0)  {
			SDL_Surface* temp = areasurf;
			
			areasurf = rotozoomSurfaceXY(temp, a->angle, 1, 1, 0);
			SDL_FreeSurface(temp);
			if (areasurf == NULL) continue;
		}
		
		dest.x = a->x;
		dest.y = a->y;
		
		SDL_BlitSurface(areasurf, NULL, surf, &dest);
		SDL_FreeSurface(areasurf);
	}
	
	return surf;
}


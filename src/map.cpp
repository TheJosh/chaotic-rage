#include <iostream>
#include <SDL.h>
#include <math.h>
#include "rage.h"

using namespace std;


SDL_Surface *createDataSurface(int w, int h, Uint32 initial_data);


Map::Map()
{
	this->ground = NULL;
	this->walls = NULL;
	this->data = NULL;
}

Map::~Map()
{
	SDL_FreeSurface(this->ground);
	SDL_FreeSurface(this->walls);
	free(this->data);
}


/**
* Load a file (simulated)
**/
int Map::load(string name)
{
	Area *a;
	SDL_Surface *datasurf;
	
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
	
	a = new Area();
	a->x = 100;
	a->y = 600;
	a->width = 600;
	a->height = 70;
	a->angle = 0;
	a->type = getAreaTypeByID(5);
	this->areas.push_back(a);
	
	
	this->ground = this->renderFrame(0, false);
	this->walls = this->renderFrame(0, true);
	this->colourkey = SDL_MapRGB(walls->format, 255, 0, 255);
	
	this->data = (data_pixel*) malloc(width * height* sizeof(data_pixel));
	
	datasurf = this->renderDataSurface();
	for (int x = 0; x < this->width; x++) {
		for (int y = 0; y < this->height; y++) {
			this->data[x * this->height + y].type = getPixel(datasurf, x, y);
			this->data[x * this->height + y].hp = 100;
		}
	}
	
	
	return 1;
}


/**
* Render a single frame of the wall animation.
*
* It is the responsibility of the caller to free the returned surface.
**/
SDL_Surface* Map::renderFrame(int frame, bool wall)
{
	// Create
	SDL_Surface* surf = SDL_CreateRGBSurface(SDL_SWSURFACE, this->width, this->height, 32, 0,0,0,0);
	
	// Colour key for frame surfaces
	int colourkey = SDL_MapRGB(surf->format, 255, 0, 255);
	SDL_SetColorKey(surf, SDL_SRCCOLORKEY, colourkey);
	SDL_FillRect(surf, NULL, colourkey);
	
	Area *a;
	AreaType *at;
	unsigned int i;
	SDL_Rect dest;
	
	// Iterate through the areas
	for (i = 0; i < this->areas.size(); i++) {
		a = this->areas[i];
		at = a->type;
		
		//if (wall != a->type->wall) continue;
		
		if (wall and ! at->wall) continue;
		
		if (! wall and at->wall) {
			if (at->ground_type != NULL) {
				at = at->ground_type;
			} else {
				continue;
			}
		}
		
		
		// Transforms (either streches or tiles)
		SDL_Surface *areasurf = at->surf;
		if (a->type->stretch)  {
			areasurf = rotozoomSurfaceXY(areasurf, 0, ((double)a->width) / ((double)areasurf->w), ((double)a->height) / ((double)areasurf->h), 0);
			if (areasurf == NULL) continue;
			
		} else {
			areasurf = tileSprite(areasurf, a->width, a->height);
			if (areasurf == NULL) continue;
		}
		
		// Rotates
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


/**
* Render teh data surface for this map
*
* It is the responsibility of the caller to free the returned surface.
**/
SDL_Surface* Map::renderDataSurface()
{
	// Create
	SDL_Surface* surf = SDL_CreateRGBSurface(SDL_SWSURFACE, this->width, this->height, 32, 0,0,0,0);
	
	Area *a;
	unsigned int i;
	SDL_Rect dest;
	SDL_Surface *datasurf;
	
	// Iterate through the areas
	for (i = 0; i < this->areas.size(); i++) {
		a = this->areas[i];
		
		datasurf = createDataSurface(a->width, a->height, a->type->id);
		
		// Transforms (either streches or tiles)
		SDL_Surface *areasurf = a->type->surf;
		if (a->type->stretch)  {
			areasurf = rotozoomSurfaceXY(areasurf, 0, ((double)a->width) / ((double)areasurf->w), ((double)a->height) / ((double)areasurf->h), 0);
			if (areasurf == NULL) continue;
			
		} else {
			areasurf = tileSprite(areasurf, a->width, a->height);
			if (areasurf == NULL) continue;
		}
		
		// Rotates
		if (a->angle != 0)  {
			SDL_Surface* temp = areasurf;
			
			areasurf = rotozoomSurfaceXY(temp, a->angle, 1, 1, 0);
			SDL_FreeSurface(temp);
			if (areasurf == NULL) continue;
			
			temp = datasurf;
			datasurf = rotozoomSurfaceXY(temp, a->angle, 1, 1, 0);
			SDL_FreeSurface(temp);
			if (datasurf == NULL) continue;
		}
		
		dest.x = a->x;
		dest.y = a->y;
		
		cross_mask(datasurf, areasurf);
		SDL_FreeSurface(areasurf);
		
		SDL_BlitSurface(datasurf, NULL, surf, &dest);
		SDL_FreeSurface(datasurf);
	}
	
	return surf;
}


/**
* Creates a data surface and fills it with the defined data
**/
SDL_Surface *createDataSurface(int w, int h, Uint32 initial_data)
{
	SDL_Surface *surf = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 32, 0,0,0,0);
	
	SDL_FillRect(surf, NULL, initial_data);
	
	return surf;
}


/**
* Gets a data pixel
**/
data_pixel Map::getDataAt(int x, int y)
{
	data_pixel ret;
	ret.type = 0;
	
	if (x < 0) return ret;
	if (y < 0) return ret;
	if (x >= this->width) return ret;
	if (x >= this->height) return ret;
	
	return this->data[x * this->height + y];
}

/**
* Sets a data pixel
**/
void Map::setDataHP(int x, int y, int newhp)
{
	if (x < 0) return;
	if (y < 0) return;
	if (x >= this->width) return;
	if (x >= this->height) return;
	
	if (newhp <= 0) {
		AreaType *at = getAreaTypeByID(this->data[x * this->height + y].type);
		
		this->data[x * this->height + y].hp = 0;
		this->data[x * this->height + y].type = (at->ground_type != NULL ? at->ground_type->id : 0);
		
		setPixel(this->walls, x, y, this->colourkey);
		
		return;
	}
	
	this->data[x * this->height + y].hp = newhp;
}



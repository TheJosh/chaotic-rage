// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include <math.h>
#include "rage.h"

using namespace std;


SDL_Surface *createDataSurface(int w, int h, Uint32 initial_data);


Area::Area(AreaType * type)
{
	this->type = type;
	
	if (type->model != NULL) {
		this->anim = new AnimPlay(type->model);
	} else {
		this->anim = NULL;
	}
}

Area::~Area()
{
	delete(anim);
}

Map::Map(GameState * st)
{
	this->st = st;
	this->background = NULL;
	this->data = NULL;
}

Map::~Map()
{
	free(this->data);
}


/**
* Load a file (simulated)
**/
int Map::load(string name, Render * render)
{
	Area *a;
	Zone *z;
	
	this->render = render;
	this->width = 1000;
	this->height = 1000;
	
	// Default area
	a = new Area(this->st->getMod(0)->getAreaType(0));
	a->x = 0;
	a->y = 0;
	a->width = this->width;
	a->height = this->height;
	a->angle = 0;
	this->areas.push_back(a);
	
	a = new Area(this->st->getMod(0)->getAreaType(2));
	a->x = 300;
	a->y = 300;
	a->width = 100;
	a->height = 100;
	a->angle = 22;
	this->areas.push_back(a);
	
	a = new Area(this->st->getMod(0)->getAreaType(2));
	a->x = 150;
	a->y = 170;
	a->width = 100;
	a->height = 100;
	a->angle = 0;
	this->areas.push_back(a);
	
	a = new Area(this->st->getMod(0)->getAreaType(3));
	a->x = 0;
	a->y = 0;
	a->width = 200;
	a->height = 1000;
	a->angle = 2;
	this->areas.push_back(a);
	
	a = new Area(this->st->getMod(0)->getAreaType(3));
	a->x = 400;
	a->y = 30;
	a->width = 200;
	a->height = 30;
	a->angle = 0;
	this->areas.push_back(a);
	
	for (int x = 100; x < 800; x += 50) {
		a = new Area(this->st->getMod(0)->getAreaType(5));
		a->x = x;
		a->y = 600;
		a->width = 25;
		a->height = 25;
		a->angle = 0;
		this->areas.push_back(a);
	}
	
	z = new Zone(50,50,60,60);
	z->spawn[FACTION_INDIVIDUAL] = 1;
	this->zones.push_back(z);
	
	z = new Zone(500,500,560,560);
	z->spawn[FACTION_INDIVIDUAL] = 1;
	this->zones.push_back(z);
	
	
	// This is a hack to use the SDL rotozoomer to manipulate the data surface
	// The data surface is created using a 32-bit SDL_Surface
	// which the areatypes are blitted onto (using rotozooming if required)
	// This is then converted into a regular array using a loop and the getPixel
	// function
	SDL_Surface *datasurf = this->renderDataSurface();
	
	this->data = (data_pixel*) malloc(width * height* sizeof(data_pixel));
	
	for (int x = 0; x < this->width; x++) {
		for (int y = 0; y < this->height; y++) {
			this->data[x * this->height + y].type = this->st->getMod(0)->getAreaType(getPixel(datasurf, x, y));		// TODO getpixel should be a pointer to the AreaType
			this->data[x * this->height + y].hp = 100;
		}
	}
	// Hack end
	
	
	Mod * mod = new Mod(st, "maps/test/");
	this->background = this->render->loadSprite("background.jpg", mod);
	
	return 1;
}


/**
* Render the data surface for this map
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
		SDL_Surface *areasurf = a->type->surf->orig;
		if (a->type->stretch)  {
			areasurf = rotozoomSurfaceXY(areasurf, 0, ((double)a->width) / ((double)areasurf->w), ((double)a->height) / ((double)areasurf->h), 0);
			if (areasurf == NULL) continue;
			
		} else {
			areasurf = (SDL_Surface *)tileSprite(areasurf, a->width, a->height);
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
* Returns an AreaType if we hit a wall, or NULL if we haven't
*
* Makes use of the AreaType check radius, as well as the checking objects'
* check radius.
*
* @tag good-for-optimise
**/
AreaType * Map::checkHit(int x, int y, int check_radius)
{
	return NULL;
}


/**
* Gets a data pixel
**/
data_pixel Map::getDataAt(int x, int y)
{
	data_pixel ret;
	ret.type = NULL;
	ret.hp = 0;
	
	if (x < 0) return ret;
	if (y < 0) return ret;
	if (x >= this->width) return ret;
	if (x >= this->height) return ret;
	
	return this->data[x * this->height + y];
}

/**
* Sets a data to a specific health value
**/
void Map::setDataHP(int x, int y, int newhp)
{
	if (x < 0) return;
	if (y < 0) return;
	if (x >= this->width) return;
	if (x >= this->height) return;
	
	if (newhp <= 0) {
		AreaType *at = this->data[x * this->height + y].type;
		
		this->data[x * this->height + y].hp = 0;
		this->data[x * this->height + y].type = at->ground_type;
		
		this->render->clearPixel(x, y);
		
		return;
	}
	
	this->data[x * this->height + y].hp = newhp;
}


/**
* Finds a spawn zone for a specific faction.
* Returns NULL if none are found.
* @todo Choose one randomly instead of grabbing the first one
**/
Zone * Map::getSpawnZone(Faction f)
{
	int num = 0;
	Zone * candidates[20];
	
	for (unsigned int i = 0; i < this->zones.size(); i++) {
		if (this->zones[i]->spawn[f] == 1) {
			candidates[num++] = this->zones[i];
			if (num == 20) break;
		}
	}
	
	if (num == 0) return NULL;
	
	num = round(getRandom(0, num * 10) / 10);	//getRandom seems to have some problems with small values
	
	return candidates[num];
}

/**
* Finds a prison zone for a specific faction.
* Returns NULL if none are found.
* @todo Choose one randomly instead of grabbing the first one
**/
Zone * Map::getPrisonZone(Faction f)
{
	for (unsigned int i = 0; i < this->zones.size(); i++) {
		if (this->zones[i]->prison[f] == 1) return this->zones[i];
	}
	
	return NULL;
}

/**
* Finds a collect zone for a specific faction.
* Returns NULL if none are found.
* @todo Choose one randomly instead of grabbing the first one
**/
Zone * Map::getCollectZone(Faction f)
{
	for (unsigned int i = 0; i < this->zones.size(); i++) {
		if (this->zones[i]->collect[f] == 1) return this->zones[i];
	}
	
	return NULL;
}

/**
* Finds a destination zone for a specific faction.
* Returns NULL if none are found.
* @todo Choose one randomly instead of grabbing the first one
**/
Zone * Map::getDestZone(Faction f)
{
	for (unsigned int i = 0; i < this->zones.size(); i++) {
		if (this->zones[i]->dest[f] == 1) return this->zones[i];
	}
	
	return NULL;
}

/**
* Finds a nearbase zone for a specific faction.
* Returns NULL if none are found.
* @todo Choose one randomly instead of grabbing the first one
**/
Zone * Map::getNearbaseZone(Faction f)
{
	for (unsigned int i = 0; i < this->zones.size(); i++) {
		if (this->zones[i]->nearbase[f] == 1) return this->zones[i];
	}
	
	return NULL;
}



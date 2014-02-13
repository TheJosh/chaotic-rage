// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include "../rage.h"
#include "../game_state.h"
#include "../map.h"
#include "render_debug.h"
#include "sprite.h"
#include <SDL_image.h>


using namespace std;


RenderDebug::RenderDebug(GameState * st) : Render(st)
{
	SDL_InitSubSystem(SDL_INIT_VIDEO);
	
	this->window = NULL;
	this->renderer = NULL;
}

RenderDebug::~RenderDebug()
{
	this->cleanup();
}


/**
* Load a texture bitmap from file
* Basically just wraps SDL_LoadBMP
**/
SDL_Texture *RenderDebug::loadTexture(const char* filename)
{
	SDL_Surface *surf = SDL_LoadBMP(filename);
	SDL_Texture *tex = SDL_CreateTextureFromSurface(this->renderer, surf);
	SDL_FreeSurface(surf);
	return tex;
}


/**
* Sets the screen size
**/
void RenderDebug::setScreenSize(int width, int height, bool fullscreen)
{
	this->width = width;
	this->height = height;
	
	if (this->window) {
		this->cleanup();
	}
	
	// Create window
	this->window = SDL_CreateWindow("Chaotic Rage (debug)", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, 0);
	if (this->window == NULL) {
		fprintf(stderr, "Unable to set %ix%i video: %s\n", width, height, SDL_GetError());
		exit(1);
	}
	
	// Create renderer
	this->renderer = SDL_CreateRenderer(this->window, -1, 0);
	if (this->renderer == NULL) {
		fprintf(stderr, "Unable to set %ix%i video: %s\n", width, height, SDL_GetError());
		exit(1);
	}
	
	// Load assets
	this->sprite_wall = this->loadTexture("data/debug/wall.bmp");
	this->sprite_vehicle = this->loadTexture("data/debug/vehicle.bmp");
	this->sprite_object = this->loadTexture("data/debug/object.bmp");
	this->sprite_unit = this->loadTexture("data/debug/unit.bmp");
	this->sprite_player = this->loadTexture("data/debug/player.bmp");
}


/**
* Free textures and window
**/
void RenderDebug::cleanup()
{
	SDL_DestroyTexture(this->sprite_wall);
	SDL_DestroyTexture(this->sprite_unit);
	SDL_DestroyTexture(this->sprite_vehicle);
	SDL_DestroyTexture(this->sprite_object);
	SDL_DestroyTexture(this->sprite_player);
	SDL_DestroyRenderer(this->renderer);
	SDL_DestroyWindow(this->window);
}


/**
* Internal sprite loading from a SDL_RWops
**/
SpritePtr RenderDebug::int_loadSprite(SDL_RWops *rw, string filename)
{
	SDL_Surface * surf;
	
	surf = IMG_Load_RW(rw, 0);
	if (surf == NULL) {
		fprintf(stderr, "Couldn't load sprite '%s'\n", filename.c_str());
		load_err = true;
		return NULL;
	}

	// Create the sprite object
	SpritePtr sprite = new Sprite();
	sprite->w = surf->w;
	sprite->h = surf->h;
	sprite->orig = surf;
	
	return sprite;
}


/**
* Renders a sprite.
* Should only be used if the the caller was called by this classes 'Render' function.
**/
void RenderDebug::renderSprite(SpritePtr sprite, int x, int y)
{
}


/**
* Renders a sprite.
* Should only be used if the the caller was called by this classes 'Render' function.
**/
void RenderDebug::renderSprite(SpritePtr sprite, int x, int y, int w, int h)
{
}


/**
* Before the game
**/
void RenderDebug::preGame()
{
	last_render = st->game_time;
}


/**
* After the game
**/
void RenderDebug::postGame()
{
}


/**
* Clears all colour from a given pixel for a given sprite
**/
void RenderDebug::clearPixel(int x, int y)
{
}


/**
* Free sprite memory
**/
void RenderDebug::freeSprite(SpritePtr sprite)
{
	delete(sprite);
}


/**
* Returns sprite width
**/
int RenderDebug::getSpriteWidth(SpritePtr sprite)
{
	return 0;
}

/**
* Returns sprite height
**/
int RenderDebug::getSpriteHeight(SpritePtr sprite)
{
	return 0;
}


/**
* Does a render every ~500ms
**/
void RenderDebug::render()
{
	if (st->game_time - last_render < 500) return;
	last_render = st->game_time;
	
	
	float scalex = ((float)st->map->width) / ((float)this->width);
	float scaley = ((float)st->map->height) / ((float)this->height);

	SDL_Rect src, dest;

	src.x = 0;
	src.y = 0;
	src.w = 16;
	src.h = 16;
	dest.w = 16;
	dest.h = 16;

	SDL_RenderClear(this->renderer);
	
	// Entities
	for (list<Entity*>::iterator it = st->entities.begin(); it != st->entities.end(); it++) {
		Entity *e = (*it);
		
		btTransform trans = e->getTransform();
		
		dest.x = (int)round(trans.getOrigin().getX() / scalex) - 8;
		dest.y = (int)round(trans.getOrigin().getZ() / scaley) - 8;
		
		if (e->klass() == WALL) {
			SDL_RenderCopy(this->renderer, this->sprite_wall, &src, &dest);
			
		} else if (e->klass() == VEHICLE) {
			SDL_RenderCopy(this->renderer, this->sprite_vehicle, &src, &dest);
			
		} else if (e->klass() == OBJECT) {
			SDL_RenderCopy(this->renderer, this->sprite_object, &src, &dest);
			
		} else if (e->klass() == UNIT) {
			if (((Unit*)e)->slot == 0) {
				SDL_RenderCopy(this->renderer, this->sprite_unit, &src, &dest);
			} else {
				SDL_RenderCopy(this->renderer, this->sprite_player, &src, &dest);
			}
		}
	}
	
	SDL_RenderPresent(this->renderer);
}



// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include "../rage.h"
#include "../game_state.h"
#include "../map/map.h"
#include "render_null.h"
#include "sprite.h"
#include <SDL.h>
#include <SDL_image.h>

using namespace std;


RenderNull::RenderNull(GameState * st) : Render(st)
{
	this->last_render = 0;
}

RenderNull::~RenderNull()
{
}


/**
* Sets the screen size
**/
void RenderNull::setScreenSize(int width, int height, bool fullscreen)
{
}


/**
* Internal sprite loading from a SDL_RWops
**/
SpritePtr RenderNull::loadSpriteFromRWops(SDL_RWops *rw, string filename)
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
void RenderNull::renderSprite(SpritePtr sprite, int x, int y)
{
}


/**
* Renders a sprite.
* Should only be used if the the caller was called by this classes 'Render' function.
**/
void RenderNull::renderSprite(SpritePtr sprite, int x, int y, int w, int h)
{
}


/**
* Before the game
**/
void RenderNull::preGame()
{
	this->last_render = st->game_time;

	cout << "======================================================" << endl;
	cout << "NEW GAME    map: " << st->map->getName() << endl;
	cout << "======================================================" << endl;
}


/**
* After the game
**/
void RenderNull::postGame()
{
}


/**
* Clears all colour from a given pixel for a given sprite
**/
void RenderNull::clearPixel(int x, int y)
{
}


/**
* Free sprite memory
**/
void RenderNull::freeSprite(SpritePtr sprite)
{
	SDL_FreeSurface(sprite->orig);
	delete(sprite);
}


/**
* Returns sprite width
**/
int RenderNull::getSpriteWidth(SpritePtr sprite)
{
	return 0;
}

/**
* Returns sprite height
**/
int RenderNull::getSpriteHeight(SpritePtr sprite)
{
	return 0;
}


/**
* This is the render method for the game
* For this renderer, we actually just dump a little bit of info about the game state
* Every ~ 5000ms
**/
void RenderNull::render()
{
	if (st->game_time - this->last_render < 5000) return;
	this->last_render = st->game_time;

	cout << "  [" << st->game_time << "]  Num entities: " << st->entities.size() << endl;
}



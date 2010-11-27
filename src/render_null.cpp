// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include "rage.h"

using namespace std;


RenderNull::RenderNull(GameState * st) : Render(st)
{
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
SpritePtr RenderNull::int_loadSprite(SDL_RWops *rw, string filename)
{
	SpritePtr p = new sprite();
	p->w = 0;
	p->h = 0;
	return p;
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
* Renders
**/
void RenderNull::render()
{
}



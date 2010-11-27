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
	cout << "The game has just started.\n";
}


/**
* After the game
**/
void RenderNull::postGame()
{
	cout << "======================================================\n";
	cout << "The game has just ended.\n";
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
* This is the render method for the game
* For this renderer, we actually just dump a little bit of info about the game state
* Every ~ 5000ms
**/
void RenderNull::render()
{
	static int last_render = st->game_time;
	
	if (st->game_time - last_render < 5000) return;
	last_render = st->game_time;
	
	int c_unit = 0, c_particle = 0, c_pgenerator = 0, c_wall = 0;
	for (unsigned int i = 0; i < st->entities.size(); i++) {
		Entity *e = st->entities.at(i);
		
		if (e->klass() == UNIT) c_unit++;
		if (e->klass() == PARTICLE) c_particle++;
		if (e->klass() == PGENERATOR) c_pgenerator++;
		if (e->klass() == WALL) c_wall++;
	}
	
	cout << "======================================================\n";
	cout << "  Current time: " << st->game_time << "\n";
	cout << "  Total num entities: " << st->entities.size() << "\n";
	cout << "  Units: " << c_unit << "\n";
	cout << "  Particles: " << c_particle << "\n";
	cout << "  Particle generators: " << c_pgenerator << "\n";
	cout << "  Walls: " << c_wall << "\n";
}



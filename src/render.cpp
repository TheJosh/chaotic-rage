// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <algorithm>
#include <SDL.h>
#include "rage.h"

using namespace std;



Render::Render()
{
	this->load_err = false;
}


/**
* Loads a sprite.
* This just opens the SDL_RWops and then passes that to the actual renderer
**/
SpritePtr Render::loadSprite(string filename)
{
	SpritePtr sprite;
	SDL_RWops *rw;
	
	DEBUG("Loading bitmap '%s'.\n", filename.c_str());
	
	rw = SDL_RWFromZZIP(filename.c_str(), "r");
	if (rw == NULL) {
		fprintf(stderr, "Couldn't load sprite '%s'.\n", filename.c_str());
		load_err = true;
		return NULL;
	}
	
	sprite = this->int_loadSprite(rw, filename);
	
	if (sprite == NULL) {
		this->load_err = false;
	}
	
	SDL_RWclose (rw);
	
	return sprite;
}

bool Render::wasLoadSpriteError()
{
	return load_err;
}

// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <algorithm>
#include <SDL.h>
#include "../rage.h"
#include "../game_state.h"
#include "../mod/mod.h"
#include "render.h"
#include "sprite.h"

using namespace std;



Render::Render(GameState * st)
{
	this->st = st;
	this->load_err = false;
}


/**
* Loads a sprite from a mod
* This just opens the SDL_RWops and then passes that to the actual renderer
**/
SpritePtr Render::loadSprite(string filename, Mod * mod)
{
	SpritePtr sprite;
	SDL_RWops *rw;
	
	DEBUG("vid", "Loading sprite '%s'", filename.c_str());
	
	rw = mod->loadRWops(filename);
	if (rw == NULL) {
		load_err = true;
		return NULL;
	}
	
	sprite = this->int_loadSprite(rw, filename);
	if (sprite == NULL) {
		this->load_err = true;
	}
	
	SDL_RWclose (rw);
	
	return sprite;
}

bool Render::wasLoadSpriteError()
{
	return load_err;
}

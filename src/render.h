// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"


class Render
{
	protected:
		bool load_err;
		
	protected:
		virtual SpritePtr int_loadSprite(SDL_RWops *rw, string filename) = 0;
		
	public:
		/**
		* Sets the screen size of this renderer
		**/
		virtual void setScreenSize(int width, int height, bool fullscreen) = 0;
		
		/**
		* Renders the provided game state
		**/
		virtual void render(GameState *st) = 0;
		
		/**
		* Loads a sprite from file into memeory
		* Uses magneta for colour key
		* The filename can be in a compressed ZIP file or a directory
		**/
		SpritePtr loadSprite(string filename);
		
		bool wasLoadSpriteError();
		
	public:
		Render();
};

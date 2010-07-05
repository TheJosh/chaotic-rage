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
		
		/**
		* Renders a sprite
		**/
		virtual void renderSprite(SpritePtr sprite, int x, int y) = 0;
		
		/**
		* Renders the background for the map
		**/
		virtual SpritePtr renderMap(Map * map, int frame, bool wall) = 0;
		
		/**
		* Clears all colour from a given pixel for a given sprite
		**/
		virtual void clearPixel(SpritePtr sprite, int x, int y) = 0;
		
		/**
		* Free sprite memory
		**/
		virtual void freeSprite(SpritePtr sprite) = 0;
		
		/**
		* Returns the width of a sprite
		**/
		virtual int getSpriteWidth(SpritePtr sprite) = 0;
		
		/**
		* Returns the height of a sprite
		**/
		virtual int getSpriteHeight(SpritePtr sprite) = 0;
		
		/**
		* Returns true if there were any sprite errors
		**/
		bool wasLoadSpriteError();
		
	public:
		Render();
};

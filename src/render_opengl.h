// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"


class RenderOpenGL : public Render
{
	private:
		SDL_Surface * screen;
		
	protected:
		virtual SpritePtr int_loadSprite(SDL_RWops *rw, string filename);
		
	public:
		virtual void setScreenSize(int width, int height, bool fullscreen);
		virtual void render(GameState *st);
		virtual void renderSprite(SpritePtr sprite, int x, int y);
		virtual SpritePtr renderMap(Map * map, int frame, bool wall);
		virtual void clearPixel(SpritePtr sprite, int x, int y);
		virtual void freeSprite(SpritePtr sprite);
		virtual int getSpriteWidth(SpritePtr sprite);
		virtual int getSpriteHeight(SpritePtr sprite);
		
	public:
		RenderOpenGL();
		~RenderOpenGL();
		
};


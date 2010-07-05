// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"


class RenderSDL : public Render
{
	private:
		SDL_Surface * screen;
		
	protected:
		virtual SpritePtr int_loadSprite(SDL_RWops *rw, string filename);
		
	public:
		virtual void setScreenSize(int width, int height, bool fullscreen);
		virtual void render(GameState *st);
		
	public:
		RenderSDL();
		~RenderSDL();
		
};


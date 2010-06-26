#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"


class RenderSDL : public Render
{
	private:
		SDL_Surface * screen;
		
	public:
		virtual void setScreenSize(int width, int height, bool fullscreen);
		virtual void render(GameState *st);
		
	public:
		RenderSDL();
		~RenderSDL();
		
};


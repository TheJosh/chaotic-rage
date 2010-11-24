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
		
		// Size of the 'virtual' screen.
		int virt_width;
		int virt_height;
		
		SpritePtr ground;
		SpritePtr walls;
		
	protected:
		virtual SpritePtr int_loadSprite(SDL_RWops *rw, string filename);
		
	private:
		SpritePtr renderMap(Map * map, int frame, bool wall);
		
	public:
		virtual void setScreenSize(int width, int height, bool fullscreen);
		virtual void render();
		virtual void renderSprite(SpritePtr sprite, int x, int y);
		virtual void preGame();
		virtual void postGame();
		virtual void clearPixel(int x, int y);
		virtual void freeSprite(SpritePtr sprite);
		virtual int getSpriteWidth(SpritePtr sprite);
		virtual int getSpriteHeight(SpritePtr sprite);
		
	public:
		RenderOpenGL(GameState * st);
		virtual ~RenderOpenGL();
		
		void renderSprite(SpritePtr sprite, int x, int y, int w, int h);
};


// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <SDL.h>
#include "../rage.h"
#include "render.h"


/**
* A really basic 2D renderer which only displays boxes.
* For debugging, etc
**/
class RenderDebug : public Render
{
	private:
		SDL_Window *window;
		SDL_Renderer *renderer;
		int last_render;

		SDL_Texture *sprite_wall;
		SDL_Texture *sprite_vehicle;
		SDL_Texture *sprite_object;
		SDL_Texture *sprite_unit;
		SDL_Texture *sprite_player;
		SDL_Texture *bg;

		int width;
		int height;

	protected:
		virtual SpritePtr loadSpriteFromRWops(SDL_RWops *rw, string filename);

	public:
		virtual void setScreenSize(int width, int height, bool fullscreen);
		virtual void render();
		virtual void renderSprite(SpritePtr sprite, int x, int y);
		virtual void renderSprite(SpritePtr sprite, int x, int y, int w, int h);
		virtual void preGame();
		virtual void postGame();
		virtual void clearPixel(int x, int y);
		virtual void freeSprite(SpritePtr sprite);
		virtual int getSpriteWidth(SpritePtr sprite);
		virtual int getSpriteHeight(SpritePtr sprite);

	public:
		RenderDebug(GameState * st);
		virtual ~RenderDebug();

	private:
		SDL_Texture *createSolidTexture(int width, int height, int r, int g, int b, int a);
		void cleanup();
};


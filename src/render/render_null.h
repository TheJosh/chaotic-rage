// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include "../types.h"
#include "render.h"


class RenderNull : public Render
{
	private:
		int last_render;

	public:
		virtual void setScreenSize(int width, int height, bool fullscreen);
		virtual void render();
		virtual SpritePtr loadSpriteFromRWops(SDL_RWops *rw, string filename);
		virtual void renderSprite(SpritePtr sprite, int x, int y);
		virtual void renderSprite(SpritePtr sprite, int x, int y, int w, int h);
		virtual void preGame();
		virtual void postGame();
		virtual void clearPixel(int x, int y);
		virtual void freeSprite(SpritePtr sprite);
		virtual int getSpriteWidth(SpritePtr sprite);
		virtual int getSpriteHeight(SpritePtr sprite);

	public:
		RenderNull(GameState * st);
		virtual ~RenderNull();
};

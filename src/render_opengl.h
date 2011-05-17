// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

#include <ft2build.h>
#include FT_FREETYPE_H

struct VBOvertex
{
	float x, y, z;        // Vertex
	float nx, ny, nz;     // Normal
	float tx, ty;         // Tex
};


class RenderOpenGL : public Render
{
	friend class HUD;
	
	private:
		SDL_Surface * screen;
		
		// Size of the actual screen
		int real_width;
		int real_height;
		
		// Size of the 'virtual' screen.
		int virt_width;
		int virt_height;
		
		SpritePtr ground;
		SpritePtr walls;
		
		AnimPlay *test;
		
		FT_Library ft;
		FT_Face face;
		
	protected:
		virtual SpritePtr int_loadSprite(SDL_RWops *rw, string filename);
		
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
		RenderOpenGL(GameState * st);
		virtual ~RenderOpenGL();
		
		void saveScreenshot(string filename);
		
	protected:
		void renderObj (WavefrontObj * obj);
		void renderAnimPlay(AnimPlay * play);
		
	private:
		void createVBO (WavefrontObj * obj);
		
};


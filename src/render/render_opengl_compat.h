// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include "../rage.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#define NUM_CHAR_TEX (128 - 32)

#include <guichan.hpp>
#include <guichan/sdl.hpp>
#include <guichan/opengl.hpp>
#include <guichan/opengl/openglsdlimageloader.hpp>

#include "LinearMath/btIDebugDraw.h"



class RenderOpenGLCompat : public Render3D
{
	friend class HUD;
	friend class Menu;
	friend class Intro;
	friend class GameState;
	
	private:
		SDL_Surface * screen;
		
		// The current player being rendered (split screen)
		Player* render_player;
		
		// FreeType
		FT_Library ft;
		FT_Face face;
		vector<SpritePtr> loaded;
		FreetypeChar char_tex[NUM_CHAR_TEX];
		
		// Heightmap
		unsigned int ter_size;
		GLuint ter_vboid;
		
		// Debugging
		AnimPlay *test;
		btIDebugDraw *physicsdebug;
		
	public:
		RenderOpenGLCompat(GameState * st);
		virtual ~RenderOpenGLCompat();
		
	protected:
		virtual SpritePtr int_loadSprite(SDL_RWops *rw, string filename);
		
	public:
		// From class Render
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
		virtual void enablePhysicsDebug();
		virtual void disablePhysicsDebug();
		virtual void loadHeightmap();
		virtual void freeHeightmap();
		virtual int getWidth() { return real_width; }
		virtual int getHeight() { return real_height; }
		
	public:
		// From class Render3D
		virtual void saveScreenshot(string filename);
		virtual void initGuichan(gcn::Gui * gui, Mod * mod);
		virtual void preVBOrender();
		virtual void postVBOrender();
		virtual void renderAnimPlay(AnimPlay * play);
		virtual void renderObj (WavefrontObj * obj);
		virtual void loadFont(string name, Mod * mod);
		virtual void renderText(string text, float x = 0.0, float y = 0.0, float r = 1.0, float g = 1.0, float b = 1.0);
		
	private:
		void renderCharacter(char c);
		void deadRot();
		void createVBO (WavefrontObj * obj);
		void surfaceToOpenGL(SpritePtr sprite);
		void mainViewport(int s, int of);
		void background();
		void mainRot();
		void lights();
		void map();
		void entities();
		void physics();
		void particles();
		void guichan();
		void hud(HUD * hud);
};



// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include "../rage.h"

#ifdef __ANDROID__
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
#else
	#include <GL/gl.h>
#endif

#include <SDL.h>


using namespace std;


struct sprite {
	int w;
	int h;
	GLuint pixels;
	SDL_Surface *orig;		// TODO: remove dependency on this...perhaps
};
typedef sprite* SpritePtr;


class GameState;


class Render
{
	protected:
		bool load_err;
		GameState * st;
		
	protected:
		virtual SpritePtr int_loadSprite(SDL_RWops *rw, string filename) = 0;
		
	public:
		int viewmode;
		
		/**
		* Sets the screen size of this renderer
		**/
		virtual void setScreenSize(int width, int height, bool fullscreen) = 0;
		
		/**
		* Renders the provided game state
		**/
		virtual void render() = 0;
		
		/**
		* Loads a sprite from a mod into memeory
		**/
		SpritePtr loadSprite(string filename, Mod * mod);
		
		/**
		* Renders a sprite.
		* Should only be used if the the caller was called by this classes 'Render' function.
		**/
		virtual void renderSprite(SpritePtr sprite, int x, int y) = 0;
		
		/**
		* Renders a sprite.
		* Should only be used if the the caller was called by this classes 'Render' function.
		**/
		virtual void renderSprite(SpritePtr sprite, int x, int y, int w, int h) = 0;
		
		/**
		* Prepare any surfaces needed before a game is played
		**/
		virtual void preGame() = 0;
		
		/**
		* Free any surfaces after the game is done
		**/
		virtual void postGame() = 0;
		
		/**
		* Clears all colour from a given pixel for a given sprite
		**/
		virtual void clearPixel(int x, int y) = 0;
		
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
		
		/**
		* Enable or disable physics debugging
		**/
		virtual void setPhysicsDebug(bool status) {}
		
		/**
		* Get current physics debug status
		**/
		virtual bool getPhysicsDebug() { return false; }
		
		/**
		* Enable or disable speed debugging
		**/
		virtual void setSpeedDebug(bool status) {}
		
		/**
		* Get current speed debug status
		**/
		virtual bool getSpeedDebug() { return false; }
		
		/**
		* Load a heightmap from an image.
		**/
		virtual void loadHeightmap() {}
		
		/**
		* Free a loaded heightmap.
		**/
		virtual void freeHeightmap() {}
		
		/**
		* Returns the width of a sprite
		**/
		virtual int getWidth() { return 0; }
		
		/**
		* Returns the height of a sprite
		**/
		virtual int getHeight() { return 0; }
		
		/**
		* Is it a 3D renderer?
		**/
		virtual bool is3D() { return false; }
		
	public:
		Render(GameState * st);
		
		bool togglePhysicsDebug();
};



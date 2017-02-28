// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include "../rage.h"
#include "render.h"
#include <glm/glm.hpp>


class HUDLabel;

namespace gcn {
	class Gui;
}
namespace SPK {
	class Renderer;
}


class Render3D : public Render
{
	friend class HUD;
	friend class Menu;
	friend class Intro;
	friend class GameState;

	public:
		// Size of the actual screen and virtual screen
		int real_width;
		int real_height;
		int virt_width;
		int virt_height;

		// Spark particle renderers
		SPK::Renderer* renderer_lines;

	public:
		Render3D(GameState *st) : Render(st) {}
		virtual ~Render3D() {}

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
		virtual void render() = 0;

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
		* Loads a cubemap from a mod into memory
		**/
		virtual SpritePtr loadCubemap(string filename_base, string filename_ext, Mod * mod) = 0;

		/**
		* Loads a 1D tex from a mod into memory
		**/
		virtual SpritePtr load1D(string filename, Mod* mod) = 0;

		/**
		* Is it a 3D renderer?
		**/
		virtual bool is3D() { return true; }


	public:
		/**
		* Save a screenshot in bmp format
		**/
		virtual void saveScreenshot(string filename) = 0;

		/**
		* Init guicahn
		**/
		virtual void initGuichan(gcn::Gui * gui, Mod * mod) = 0;

		/**
		* Load the given ttf font
		**/
		virtual void loadFont(string name, Mod * mod) = 0;

		/**
		* Render some text
		**/
		virtual void renderText(string text, int x = 0, int y = 0, float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f) = 0;

		/**
		* Render some text
		**/
		virtual unsigned int widthText(string text) = 0;

		/**
		* Calculate the raycast start and end vectors in world space for mouse picking
		**/
		virtual void mouseRaycast(int x, int y, btVector3& start, btVector3& end) = 0;

};

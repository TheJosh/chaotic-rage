// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <glm/glm.hpp>
#include "../rage.h"


using namespace std;

class AnimPlay;
class Entity;
class GameState;
class Light;
class Mod;

namespace SPK {
	class Renderer;
}


class Render
{
	protected:
		bool load_err;
		GameState * st;
		int viewmode;

	public:
		/**
		* Sets the screen size of this renderer
		**/
		virtual void setScreenSize(int width, int height, bool fullscreen) = 0;

		/**
		* Sets the view mode (above, behind, etc)
		**/
		virtual void setViewMode(int viewmode);

		/**
		* Gets the view mode (above, behind, etc)
		**/
		int getViewMode() { return this->viewmode; }

		/**
		* Set the mouse grab mode; default method does nothing
		**/
		virtual void setMouseGrab(bool newval) {}

		/**
		* Renders the provided game state
		**/
		virtual void render() = 0;

		/**
		* Loads a sprite from a mod into memory
		**/
		SpritePtr loadSprite(string filename, Mod * mod);

		virtual SpritePtr loadSpriteFromRWops(SDL_RWops *rw, string filename) = 0;

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

		/**
		* Add an animation to the renderer
		* Any animations left over at game end will be delete()ed
		**/
		virtual void addAnimPlay(AnimPlay* play, Entity* e) {}

		/**
		* Remove an animation from the renderer
		**/
		virtual void remAnimPlay(AnimPlay* play) {}

		/**
		* Add a light to the renderer
		* Any lights left over at game end will be delete()ed
		**/
		virtual void addLight(Light* light) {}

		/**
		* Remove a light from the renderer
		**/
		virtual void remLight(Light* light) {}

		/**
		* Set the state of the torch
		**/
		virtual void setTorch(bool on) {}

		/**
		* Set ambient light
		**/
		virtual void setAmbient(glm::vec4 ambient) {}

		/**
		* Load data common across all games
		**/
		virtual void loadCommonData() {}

		/**
		* Add a particle renderer; these get updated with view/projection matrixes each frame
		**/
		virtual void addParticleRenderer(SPK::Renderer* r) {}

	public:
		Render(GameState * st);
		virtual ~Render() {}

		bool togglePhysicsDebug();
};

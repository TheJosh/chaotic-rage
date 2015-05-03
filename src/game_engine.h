// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include <list>

#include "rage.h"

namespace gcn {
	class Gui;
	class SDLInput;
	class Container;
}


/**
* Size of the frames-per-second ringbuffer
**/
#define FPS_SAMPLES 100


/**
* Contains pointers to all of the subsystems
*
* How to decide if it belongs here or in the GameState class:
*  - If it should be network-replicated, it belongs over there
*  - If it is local to this instance, it belongs here
**/
class GameEngine
{
	protected:
		// FPS ringbuffer + average
		int ticksum;
		int tickindex;
		int ticklist[FPS_SAMPLES];

		// Is the mouse currently "grabbed"?
		bool mouse_grabbed;

		// Guichan dialogs
		std::list<Dialog*> dialogs;

	public:
		// Is the game running?
		bool running;

		// Pointers to all of the subsystems
		Render* render;
		Audio* audio;
		NetServer* server;
		NetClient* client;
		CommandLineArgs* cmdline;
		ClientConfig* cconf;
		ModManager* mm;

		// Guichan
		gcn::Gui* gui;
		gcn::SDLInput* guiinput;
		gcn::Container* guitop;
		float gui_scale;

	public:
		GameEngine();
		~GameEngine();

	private:
		// Disable copy-constructor and copy-assignment
		GameEngine(const GameEngine& that);
		GameEngine& operator= (GameEngine that);

	public:
		// Mouse grab
		void setMouseGrab(bool reset);
		bool getMouseGrab();

		// Init guichan
		void initGuichan();

		// Dialog management
		bool hasDialog(DialogName name);
		Dialog* getDialog(DialogName name);
		void addDialog(Dialog * dialog);
		void remDialog(Dialog * dialog);
		bool hasDialogs();

		// Frames-per-second calcs
		void calcAverageTick(int newtick);
		float getAveTick();
};


/**
* Returns a pointer to the global GameEngine object
**/
GameEngine* GEng();

#endif // GAME_ENGINE_H

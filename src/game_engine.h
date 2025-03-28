// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include <list>
#include "types.h"

class Audio;
class ClientConfig;
class CommandLineArgs;
class Dialog;
class ModManager;
class NetClient;
class NetServer;
class Render;

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
		float ticksum;
		int tickindex;
		int tickcount;
		float ticklist[FPS_SAMPLES];

		// Is the mouse currently "grabbed"?
		bool mouse_grabbed;

		// Guichan dialogs
		std::list<Dialog*> dialogs;

	public:
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
		void calcAverageTick(float newtick);
		float getAveTick();
};


/**
* Returns a pointer to the global GameEngine object
**/
GameEngine* GEng();

#endif // GAME_ENGINE_H

// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include "rage.h"

using namespace std;


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
		int ticksum;
		int tickindex;
		int ticklist[FPS_SAMPLES];

	public:
		bool running;

		Render* render;
		Audio* audio;
		GameLogic* logic;
		NetClient* client;
		NetServer* server;
		PhysicsBullet* physics;
		CommandLineArgs* cmdline;
		ClientConfig* cconf;
		ServerConfig* sconf;
		ModManager* mm;

		bool reset_mouse;

		list<Dialog*> dialogs;
		gcn::Gui* gui;
		gcn::SDLInput* guiinput;
		gcn::Container* guitop;
		
	public:
		GameEngine();
		~GameEngine();
		
	public:
		// Mouse reset
		void setMouseGrab(bool reset);
		bool getMouseGrab();

		// GUI
		void initGuichan();
		bool hasDialog(string name);
		void addDialog(Dialog * dialog);
		void remDialog(Dialog * dialog);
		bool hasDialogs();

		// Frames-per-second
		void calcAverageTick(int newtick);
		float getAveTick();
};


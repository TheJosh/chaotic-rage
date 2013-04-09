// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include <guichan.hpp>
#include "rage.h"

using namespace std;


class MenuDialog;
class DialogNewGame;
class VectorListModel;


enum MenuCommand {
	MC_NOTHING = 0,
	MC_CAMPAIGN,
	MC_SINGLEPLAYER,
	MC_SPLITSCREEN,
	MC_NETWORK,
	MC_MODS,
	MC_HELP,
	MC_QUIT,
};

class MenuItem {
	public:
		string name;
		int x1, x2;
		int y1, y2;
		MenuCommand cmd;
		bool hover;
};


class Menu : public UIUpdate
{
	private:
		GameState *st;
		Render3D *render;
		int running;
		vector<MenuItem*> menuitems;
		gcn::SDLInput* input;
		gcn::Gui* gui;
		Dialog *dialog;
		
		WavefrontObj * bgmesh;
		float bg_rot1_pos;
		float bg_rot1_dir;
		float bg_rot2_pos;
		float bg_rot2_dir;
		
	public:
		// Data for the dialogs
		MapRegistry * mapreg;
		vector<string> gametypes;
		vector<string> viewmodes;
		vector<string> unittypes;
		ModManager *mm;
		SpritePtr logo;
		SpritePtr bg;
		bool has_campaigns;

	public:
		// Init etc
		Menu(GameState *st);
		GameState *getGameState() { return this->st; }
		void loadModBits();
		void doit();
		
	public:
		// Main menu
		void menuClear();
		void menuAdd(string name, int x, int y, MenuCommand cmd);
		void menuRender();
		void menuHover(int x, int y);
		MenuCommand menuClick(int x, int y);
		
		// Main menu actions
		void doCampaign();
		void doSingleplayer();
		void doSplitscreen();
		void doNetwork();
		void doMods();
		void doHelp();
		void doQuit();
		
		// Game dialogs
		void setDialog(Dialog * dialog);
		void startCampaign(Campaign* c, string unittype, int viewmode, unsigned int num_local);
		void startGame(MapReg *map, string gametype, string unittype, int viewmode, unsigned int num_local);
		void networkJoin(string host);
		
	public:
		virtual void updateUI();
};


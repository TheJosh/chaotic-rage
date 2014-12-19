// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <guichan.hpp>
#include <guichan/sdl.hpp>

#include "../rage.h"
#include "../util/ui_update.h"
#include "../game_settings.h"


using namespace std;


class MenuDialog;
class DialogNewGame;
class VectorListModel;
class Render3D;
class RenderOpenGL;
class GameState;
class GameManager;
class GameSettings;
class OpenGLFont;


enum MenuCommand {
	MC_NOTHING = 0,
	MC_CAMPAIGN,
	MC_SINGLEPLAYER,
	MC_SPLITSCREEN,
	MC_NETWORK,
	MC_SETTINGS,
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
		GameManager *gm;
		Mod *base_mod;
		RenderOpenGL *render;
		OpenGLFont* font;
		int running;
		vector<MenuItem*> menuitems;
		vector<Dialog*> openDialogs;

		gcn::SDLInput* input;
		gcn::Gui* gui;
		gcn::Container* gui_container;

		// TODO: Mod configuration for this model (multiple?)
		AssimpModel* model;
		AnimPlay* play;
		float model_rot;		// TODO: Proper timing for the animation

	public:
		// Data for the dialogs
		SpritePtr logo;
		SpritePtr bg;
		bool has_campaigns;

	public:
		// Init etc
		Menu(GameState *st, GameManager *gm);
		~Menu();
		GameState *getGameState() { return this->st; }

		void loadModBits(UIUpdate* ui = NULL);
		void loadMenuItems();
		void createBGmesh();
		void doit(UIUpdate* ui);
		void setupGLstate();
		virtual void updateUI();

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
		void doSettings();
		void doMods();
		void doHelp();
		void doQuit();

		// Game dialogs
		void addDialog(Dialog * dialog);
		void remDialog(Dialog * dialog);
		void remAllDialogs();

		// Start game stuff
		// TODO: Remove this and go direct to GameManager
		void startCampaign(Campaign* c, string unittype, GameSettings::ViewMode viewmode, unsigned int num_local);
		void startGame(MapReg *map, string gametype, string unittype, GameSettings::ViewMode viewmode, unsigned int num_local, bool host, GameSettings* gs);
		void networkJoin(string host);
		
		// Handle a res change from the settings screen
		void handleScreenResChange();
};

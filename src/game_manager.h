// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <vector>
#include <string>
#include <list>
#include "game_settings.h"

using namespace std;


class GameState;
class Campaign;
class MapReg;
class MapRegistry;
class UIUpdate;
class GameSettings;


class GameManager
{
	private:
		GameState *st;
		MapRegistry *mapreg;
		vector<string> gametypes;
		vector<string> viewmodes;
		vector<string> unittypes;

	public:
		// Init etc
		explicit GameManager(GameState *st);
		~GameManager();

	public:
		/**
		* Load stuff from mods into arrays
		**/
		void loadModBits(UIUpdate* ui);

		/**
		* Getting stuff from the mod
		**/
		MapRegistry* getMapRegistry();
		vector<string>* getGameTypes();
		vector<string>* getViewModes();
		vector<string>* getUnitTypes();

		/**
		* Run a campaign
		**/
		void startCampaign(Campaign* c, string unittype, GameSettings::ViewMode viewmode, unsigned int num_local);

		/**
		* Run an arcade game
		**/
		void startGame(MapReg *map, string gametype, string unittype, GameSettings::ViewMode viewmode, unsigned int num_local, bool host, GameSettings *gs);

		/**
		* Attempt to join a network game
		**/
		void networkJoin(string host, UIUpdate *ui);
};


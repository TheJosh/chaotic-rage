// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include "rage.h"

using namespace std;



/**
* Per-faction game settings
**/
class GameSettingsFaction
{
	public:
		bool spawn_weapons_unit;
		bool spawn_weapons_gametype;
		vector<WeaponType*> spawn_weapons_extra;

	public:
		GameSettingsFaction();
};


/**
* Game settings
**/
class GameSettings
{
	public:
		GameSettingsFaction factions[NUM_FACTIONS];

	public:
		GameSettings();
};

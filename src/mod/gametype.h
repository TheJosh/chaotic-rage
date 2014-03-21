// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include "../rage.h"

using namespace std;


/**
* Per-faction settings for a gametype
**/
class GameTypeFaction
{
	public:
		string title;
		vector<WeaponType*> spawn_weapons;
};


/**
* A gametype, including script and game settings
**/
class GameType
{
	public:
		string name;
		CRC32 id;
		string title;

		string script;
		GameTypeFaction factions[NUM_FACTIONS];
		unsigned num_factions;
		
	public:
		GameType();
		
	public:
		bool operator < (const GameType& other) const
		{
			return (title < other.title);
		}
};


// Config file opts
extern cfg_opt_t gametype_opts [];

// Item loading function handler
GameType* loadItemGameType(cfg_t* cfg_item, Mod* mod);

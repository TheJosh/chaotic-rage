// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <confuse.h>
#include "../rage.h"
#include "mod.h"
#include "gametype.h"
#include "game_engine.h"
#include "mod_manager.h"

using namespace std;


/**
* Faction settings
**/
cfg_opt_t gametype_faction_opts[] =
{
	CFG_INT((char*) "id", 0, CFGF_NONE),
	CFG_STR((char*) "title", 0, CFGF_NONE),
	CFG_STR_LIST((char*) "spawn_weapons", 0, CFGF_NONE),
	CFG_END()
};


/**
* Config file opts
**/
cfg_opt_t gametype_opts[] =
{
	CFG_STR((char*) "name", 0, CFGF_NONE),
	CFG_STR((char*) "title", 0, CFGF_NONE),
	CFG_STR((char*) "script", 0, CFGF_NONE),
	CFG_SEC((char*) "faction", gametype_faction_opts, CFGF_MULTI),
	CFG_END()
};


/**
* Item loading function handler
**/
GameType* loadItemGameType(cfg_t* cfg_item, Mod* mod)
{
	GameType* gt;
	string filename;
	cfg_t* cfg_faction;
	int j, k;


	gt = new GameType();
	gt->name = cfg_getstr(cfg_item, "name");
	gt->title = cfg_getstr(cfg_item, "title");
	
	filename = "gametypes/";
	filename.append(cfg_getstr(cfg_item, "script"));
	filename.append(".lua");
	
	char * tmp = mod->loadText(filename);
	if (tmp == NULL) {
		mod->setLoadErr("Invalid gametype stage, no 'script' specified");
		return NULL;
	}
	
	gt->script = std::string(tmp);
	free(tmp);


	// Factions
	vector<WeaponType*> weaps;
	int num_factions = cfg_size(cfg_item, "faction");
	for (j = 0; j < num_factions; j++) {
		cfg_faction = cfg_getnsec(cfg_item, "faction", j);
		
		// Check faction id is okay
		int faction_id = cfg_getint(cfg_faction, "id");
		if (faction_id < 0 || faction_id >= NUM_FACTIONS) {
			mod->setLoadErr("Invalid faction id %i", faction_id);
			return NULL;
		}

		// Load faction settings
		gt->factions[faction_id].title = cfg_getstr(cfg_faction, "title");

		// Load spawn weapons
		int num_weapons = cfg_size(cfg_faction, "spawn_weapons");
		for (k = 0; j < num_weapons; j++) {
			WeaponType * wt = mod->getWeaponType(cfg_getnstr(cfg_faction, "spawn_weapons", j));
			if (wt == NULL) {
				mod->setLoadErr("Invalid spawn weapon %s for action %i", cfg_getnstr(cfg_faction, "spawn_weapons", j), faction_id);
				return NULL;
			}

			gt->factions[faction_id].spawn_weapons.push_back(wt);
		}
	}


	return gt;
}


GameType::GameType()
{
}

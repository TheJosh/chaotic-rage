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

	gt = new GameType();
	gt->name = cfg_getstr(cfg_item, "name");
	gt->title = cfg_getstr(cfg_item, "title");
	gt->mod = mod;

	filename = "gametypes/";
	filename.append(cfg_getstr(cfg_item, "script"));
	filename.append(".lua");
	gt->script_filename = filename;

	int num_factions = cfg_size(cfg_item, "faction");
	if (num_factions == 0) {
		// Default faction titles - only if no faction names provided
		gt->factions[0].title = "Individuals";
		for (int i = 1; i < NUM_FACTIONS; i++) {
			char str[50];
			sprintf(str, "Faction %i", i);
			gt->factions[i].title = std::string(str);
		}
		gt->num_factions = NUM_FACTIONS;

	} else {
		// All names should be explictly provided, any which are not are assumed to not be used.
		for (int i = 0; i < NUM_FACTIONS; i++) {
			gt->factions[i].title = "unused";
		}
		gt->num_factions = 0;

		// Load faction details
		for (int j = 0; j < num_factions; j++) {
			cfg_t* cfg_faction = cfg_getnsec(cfg_item, "faction", j);

			// Check faction id is okay
			int faction_id = cfg_getint(cfg_faction, "id");
			if (faction_id < 0 || faction_id >= NUM_FACTIONS) {
				mod->setLoadErr("Invalid faction id %i", faction_id);
				return NULL;
			}

			// The highest-numbered faction determines how many we iterate in UI etc
			if (gt->num_factions <= (unsigned int)faction_id) {
				gt->num_factions = faction_id + 1;
			}

			// Faction title
			char * tmp = cfg_getstr(cfg_faction, "title");
			if (tmp) {
				gt->factions[faction_id].title = std::string(tmp);
			}

			// Load spawn weapons
			int num_weapons = cfg_size(cfg_faction, "spawn_weapons");
			for (int k = 0; k < num_weapons; k++) {
				WeaponType * wt = mod->getWeaponType(cfg_getnstr(cfg_faction, "spawn_weapons", k));
				if (wt == NULL) {
					mod->setLoadErr("Invalid spawn weapon %s for action %i", cfg_getnstr(cfg_faction, "spawn_weapons", k), faction_id);
					return NULL;
				}

				gt->factions[faction_id].spawn_weapons.push_back(wt);
			}
		}
	}


	return gt;
}


GameType::GameType()
{
}


/**
* Return the lua script code
* Please free() this when you are done
**/
char* GameType::getLuaScript() const
{
	return mod->loadText(script_filename);
}


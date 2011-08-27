// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


class WeaponType
{
	public:
		ParticleGenType *pg;
		SpritePtr icon_large;
		bool melee;
		int damage;
		
		Sound *sound_fire;
		
		// dynamic
		string name;
		int id;
		
	public:
		WeaponType();
};


// Config file opts
extern cfg_opt_t weapontype_opts [];

// Item loading function handler
WeaponType* loadItemWeaponType(cfg_t* cfg_item, Mod* mod);


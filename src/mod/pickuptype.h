// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include <list>
#include "../rage.h"

using namespace std;


#define PICKUP_TYPE_WEAPON 1
#define PICKUP_TYPE_AMMO 2


class PickupType
{
	public:
		int id;
		string name;
		AssimpModel * model;
		int type;
		
		WeaponType *weapon;       // Weapon, ammo
		
		
	public:
		PickupType();
		~PickupType();
};


// Config file opts
extern cfg_opt_t pickuptype_opts [];

// Item loading function handler
PickupType* loadItemPickupType(cfg_t* cfg_item, Mod* mod);

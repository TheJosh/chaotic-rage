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
#define PICKUP_TYPE_POWERUP 3


/**
* The amount, in percent, to adjust player attributes
**/
class PickupTypeAdjust
{
	public:
		float health;
};


/**
* The pickup details
**/
class PickupType
{
	public:
		int id;
		string name;
		string title;
		AssimpModel * model;
		int type;
		
		// Weapon and ammo crates
		WeaponType* wt;
		
		// Powerups, permanant and temporary changes
		PickupTypeAdjust* perm;
		PickupTypeAdjust* temp;
		
	public:
		PickupType();
		~PickupType();
		PickupTypeAdjust* loadAdjust(cfg_t* cfg);

		/**
		* Called by the unit when it interacts with a pickup
		**/
		void doUse(Unit *u);
};


// Config file opts
extern cfg_opt_t pickuptype_opts [];

// Item loading function handler
PickupType* loadItemPickupType(cfg_t* cfg_item, Mod* mod);

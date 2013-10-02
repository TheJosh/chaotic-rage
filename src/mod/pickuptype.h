// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <list>
#include "../rage.h"

using namespace std;


#define PICKUP_TYPE_WEAPON 1
#define PICKUP_TYPE_AMMO 2
#define PICKUP_TYPE_POWERUP 3


class btCollisionShape;


/**
* The amount, in percent, to adjust player attributes
**/
class PickupTypeAdjust
{
	public:
		float health;
		float max_speed;
		float melee_damage;
		float melee_delay;
		float melee_cooldown;
};


/**
* The pickup details
**/
class PickupType
{
	public:
		string name;
		CRC32 id;
		
		AssimpModel * model;
		btCollisionShape* col_shape;
		int type;
		
		// Weapon and ammo crates
		WeaponType* wt;
		
		// Powerups, permanant and temporary changes
		PickupTypeAdjust* perm;
		PickupTypeAdjust* temp;
		string title;
		int delay;
		
	public:
		PickupType();
		~PickupType();
		PickupTypeAdjust* loadAdjust(cfg_t* cfg);

		/**
		* Called by the unit when it interacts with a pickup
		**/
		void doUse(Unit *u);
		
		/**
		* Called by the unit when the powerup is finished
		**/
		void finished(Unit *u);
		
		/**
		* Return the delay (before we mark as finished) in ms
		**/
		int getDelay() { return this->delay; }
};


// Config file opts
extern cfg_opt_t pickuptype_opts [];

// Item loading function handler
PickupType* loadItemPickupType(cfg_t* cfg_item, Mod* mod);


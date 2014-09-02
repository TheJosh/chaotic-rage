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
#define PICKUP_TYPE_CURSOR 4
#define PICKUP_TYPE_COMBO 5

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
		bool invincible;
		float weapon_damage;

	public:
		PickupTypeAdjust():
			health(1.0f), max_speed(1.0f), melee_damage(1.0f), melee_delay(1.0f), melee_cooldown(1.0f), invincible(false), weapon_damage(1.0f)
			{}
};


/**
* Additional powerups which are applied if you get two at once
**/
class PowerupCombo
{
	public:
		string second_name;
		string benefit_name;
		PickupType* second;		// Other powerup in the combo
		PickupType* benefit;	// WHat you get from the combo
		
	public:
		PowerupCombo()
			: second_name(""), benefit_name(""), second(NULL), benefit(NULL)
			{}
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
		string message;
		int delay;

		// Powerup combos
		vector<PowerupCombo> combos;

	public:
		PickupType();
		~PickupType();
		PickupTypeAdjust* loadAdjust(cfg_t* cfg);
		void setModel(AssimpModel *model);

		/**
		* Called by the unit when it interacts with a pickup
		**/
		bool doUse(Unit *u);

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


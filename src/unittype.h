// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include <confuse.h>
#include "rage.h"

using namespace std;


#define UNIT_NUM_MODIFIERS 3
#define UNIT_MOD_CLOAK (1 << 0)
#define UNIT_MOD_SHIELD (1 << 1)
#define UNIT_MOD_SPEED (1 << 2)

#define UNIT_STATE_WALK 0		// this is going to be removed
#define UNIT_STATE_STATIC 1
#define UNIT_STATE_RUNNING 2
#define UNIT_STATE_FIRING 3
#define UNIT_STATE_DIE 4

#define UNIT_SOUND_STATIC 1
#define UNIT_SOUND_HIT 2
#define UNIT_SOUND_DEATH 3
#define UNIT_SOUND_SPAWN 4
#define UNIT_SOUND_ABILITY 5
#define UNIT_SOUND_CELEBRATE 6
#define UNIT_SOUND_FAIL 7


class UnitTypeSettings;
class UnitTypeStates;
class UnitType;


class UnitTypeSettings
{
	public:
		int max_speed;		// max pps unit can move
		int accel;		// pps/second
		int turn_move;		// degrees/second
		int turn_aim;		// degrees/second
};

class UnitTypeState
{
	public:
		int id;
		AnimModel * model;
		int type;
};

class UnitTypeSound
{
	public:
		int id;
		Sound * snd;
		int type;
};

class UnitType
{
	friend UnitType* loadItemUnitType(cfg_t* cfg_item, Mod* mod);
	
	public:
		string name;
		int id;
		int width, height;
		int begin_health;
		ParticleGenType* hit_generator;
		vector<WeaponType*> spawn_weapons;
		int playable;
		
	private:
		UnitTypeSettings initial;
		UnitTypeSettings modifiers[UNIT_NUM_MODIFIERS];
		vector<UnitTypeState*> states;
		vector<UnitTypeSound*> sounds;
		unsigned int max_frames;
		Mod * mod;
		
	public:
		UnitType();
		~UnitType();
		
	public:
		UnitTypeSettings* getSettings(Uint8 modifier_flags);
		UnitTypeState* getState(int type);
		Sound* getSound(int type);
		
		Mod * getMod() { return this->mod; }
};


// Config file opts
extern cfg_opt_t unittype_opts [];

// Item loading function handler
UnitType* loadItemUnitType(cfg_t* cfg_item, Mod* mod);



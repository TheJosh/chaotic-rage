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

#define UNIT_STATE_WALK 0
#define UNIT_STATE_STATIC 1
#define UNIT_STATE_RUNNING 2
#define UNIT_STATE_FIRING 3
#define UNIT_STATE_DIE 4


class UnitTypeSettings;
class UnitTypeStates;
class UnitType;


vector<UnitType*> * loadAllUnitTypees(Mod * mod);
UnitType* loadUnitType(cfg_t *cfg, Mod * mod);


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

class UnitType : public EntityType
{
	friend UnitType* loadUnitType(cfg_t *cfg, Mod * mod);
	
	public:
		string name;
		int id;
		int width, height;
		int begin_health;
		ParticleGenType* hit_generator;
		int playable;
		
	private:
		UnitTypeSettings initial;
		UnitTypeSettings modifiers[UNIT_NUM_MODIFIERS];
		vector<UnitTypeState*> states;
		unsigned int max_frames;
		Mod * mod;
		
	public:
		UnitType();
		~UnitType();
		
	public:
		UnitTypeSettings* getSettings(Uint8 modifier_flags);
		UnitTypeState* getState(int type);
		Mod * getMod() { return this->mod; }
};



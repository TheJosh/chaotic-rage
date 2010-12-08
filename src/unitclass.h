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


class UnitClassSettings;
class UnitClassStates;
class UnitClass;


vector<UnitClass*> * loadAllUnitClasses(Mod * mod);
UnitClass* loadUnitClass(cfg_t *cfg, Mod * mod);


class UnitClassSettings
{
	public:
		int max_speed;		// max pps unit can move
		int accel;		// pps/second
		int turn_move;		// degrees/second
		int turn_aim;		// degrees/second
};

class UnitClassState
{
	public:
		int id;
		AnimModel * model;
		int type;
};

class UnitClass : public EntityType
{
	friend UnitClass* loadUnitClass(cfg_t *cfg, Mod * mod);
	
	public:
		string name;
		int id;
		int width, height;
		int begin_health;
		
	private:
		UnitClassSettings initial;
		UnitClassSettings modifiers[UNIT_NUM_MODIFIERS];
		vector<UnitClassState*> states;
		unsigned int max_frames;
		Mod * mod;
		
	public:
		UnitClass();
		~UnitClass();
		
	public:
		UnitClassSettings* getSettings(Uint8 modifier_flags);
		UnitClassState* getState(int type);
		Mod * getMod() { return this->mod; }
};



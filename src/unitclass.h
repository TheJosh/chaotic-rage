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

class UnitClassSettings;
class UnitClass;


bool loadAllUnitClasses();
UnitClass* loadUnitClass(cfg_t *cfg);
UnitClass* getUnitClassByID(int id);


class UnitClassSettings
{
	public:
		int lin_speed;		// max pps unit can move
		int lin_accel;		// pps/second
		int turn_speed;		// degrees/second
};

class UnitClass
{
	friend UnitClass* loadUnitClass(cfg_t *cfg);
	
	public:
		string name;
		int id;
		
	private:
		UnitClassSettings initial;
		UnitClassSettings mod[UNIT_NUM_MODIFIERS];
		
	public:
		UnitClassSettings* getSettings(Uint8 modifier_flags);
};

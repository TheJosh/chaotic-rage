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

#define UNIT_STATE_STATIC 1
#define UNIT_STATE_RUN 2


class UnitClassSettings;
class UnitClassStates;
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

class UnitClassState
{
	public:
		string name;
		int type;
		unsigned int num_frames;
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
		int j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z;
		vector<UnitClassState*> states;
		
	public:
		UnitClass();
		~UnitClass();
		
	public:
		UnitClassSettings* getSettings(Uint8 modifier_flags);
		UnitClassState* getState(int type);
		vector<SDL_Surface*>* loadAllSprites();
};



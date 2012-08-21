// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include "../rage.h"

using namespace std;


class VehicleTypeDamage {
	public:
		int health;
		AnimModel * model;
};

class VehicleType
{
	public:
		int id;
		
		string name;
		AnimModel * model;
		int health;
		
		vector <VehicleTypeDamage *> damage_models;
		
	public:
		VehicleType();
};


// Config file opts
extern cfg_opt_t vehicletype_opts [];

// Item loading function handler
VehicleType* loadItemVehicleType(cfg_t* cfg_item, Mod* mod);


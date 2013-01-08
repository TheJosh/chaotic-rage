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
		bool land;
		bool water;
		
		float engine_accel;
		float engine_max;
		float brake_accel;
		float brake_max;
		float reverse_accel;
		float reverse_max;
		
		vector <VehicleTypeDamage *> damage_models;
		
	public:
		VehicleType();
};


// Config file opts
extern cfg_opt_t vehicletype_opts [];

// Item loading function handler
VehicleType* loadItemVehicleType(cfg_t* cfg_item, Mod* mod);


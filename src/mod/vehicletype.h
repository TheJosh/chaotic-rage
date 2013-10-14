// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include "../rage.h"
#include <glm/glm.hpp>

using namespace std;


class btCollisionShape;

class VehicleTypeDamage {
	public:
		int health;
		AssimpModel * model;
};

class VehicleType
{
	public:
		string name;
		CRC32 id;
		
		AssimpModel * model;
		btCollisionShape* col_shape;
		int health;
		
		bool land;
		bool water;
		bool helicopter;
		bool engine;
		bool steer;
		
		float engine_accel;
		float engine_max;
		float brake_accel;
		float brake_max;
		float reverse_accel;
		float reverse_max;
		
		string horiz_move_node;
		glm::vec3 horiz_move_axis;
		
		string vert_move_node;
		glm::vec3 vert_move_axis;
		
		WeaponType* weapon_primary;
		vector <VehicleTypeDamage *> damage_models;
		
	public:
		VehicleType();
		~VehicleType();
};


// Config file opts
extern cfg_opt_t vehicletype_opts [];

// Item loading function handler
VehicleType* loadItemVehicleType(cfg_t* cfg_item, Mod* mod);


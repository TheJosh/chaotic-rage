// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include "../rage.h"
#include <glm/glm.hpp>

using namespace std;


class btCollisionShape;
class AssimpModel;
class AssimpNode;


/**
* Models to show at various damage levels
**/
class VehicleTypeDamage {
	public:
		int health;
		AssimpModel * model;
};

/**
* Types of nodes
**/
enum VehicleNodeType {
	VEHICLE_NODE_HORIZ = 1,
	VEHICLE_NODE_VERT = 2,
	VEHICLE_NODE_SPIN = 3,
};

/**
* Move nodes, spin nodes, wheel nodes
**/
class VehicleTypeNode {
	public:
		AssimpNode * node;
		glm::vec3 axis;
		VehicleNodeType type;
};


/**
* The vehicle type itself
**/
class VehicleType
{
	public:
		string name;
		CRC32 id;
		
		AssimpModel * model;
		btCollisionShape* col_shape;
		int health;
		float mass;

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
		
		WeaponType* weapon_primary;
		vector <VehicleTypeDamage *> damage_models;
		vector <VehicleTypeNode> nodes;

		glm::vec3 joint_front;
		glm::vec3 joint_back;
		
	public:
		VehicleType();
		~VehicleType();
		
	public:
		bool hasNode(VehicleNodeType type);
		VehicleTypeNode* getNode(VehicleNodeType type);
};


// Config file opts
extern cfg_opt_t vehicletype_opts [];

// Item loading function handler
VehicleType* loadItemVehicleType(cfg_t* cfg_item, Mod* mod);


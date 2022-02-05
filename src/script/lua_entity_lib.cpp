// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;


#include <stdio.h>
#include <math.h>
#include "lua_libs.h"
#include "../game_engine.h"
#include "../entity/vehicle.h"
#include "../entity/object.h"
#include "../entity/unit/unit.h"
#include "../entity/unit/player.h"
#include "../entity/unit/npc.h"

extern "C" {
	#include <lua.h>
	#include <lauxlib.h>
}

#include "LuaBridge/LuaBridge.h"



/**
* The entity library provides classes for entity management - vehicles, pickups, etc.
*
* TODO: Expose more stuff
* TODO: Should we allow the type to be changed on an an entity
* TODO: Should we expose and allow types to be modified
**/
void load_entity_lib(lua_State *L)
{
	luabridge::getGlobalNamespace(L)
	.beginNamespace("entity")

		// Entity
		.beginClass<Entity>("Entity")
			.addFunction("remove", &Entity::remove)
			.addProperty("position", &Entity::getPositionByVal, &Entity::setPosition)
		.endClass()

		// Entity : Vehicle
		.deriveClass<Vehicle, Entity>("Vehicle")
			.addFunction("takeDamage", &Vehicle::takeDamage)
		.endClass()

		// Entity : Pickup
		.deriveClass<Pickup, Entity>("Pickup")
		.endClass()

		// Entity : Object
		.deriveClass<Object, Entity>("Object")
			.addFunction("takeDamage", &Object::takeDamage)
		.endClass()

		// Entity : Unit
		.deriveClass<Unit, Entity>("Unit")
			.addData("faction", &Unit::fac)
			.addData("slot", &Unit::slot)
			.addFunction("takeDamage", &Unit::takeDamage)
			.addFunction("die", &Unit::die)
		.endClass()

		// Entity : Unit : Player
		.deriveClass<Player, Unit>("Player")
		.endClass()

		// Entity : Unit : NPC
		.deriveClass<NPC, Unit>("NPC")
		.endClass()

	.endNamespace();
}

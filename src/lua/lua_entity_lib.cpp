// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;


#include <stdio.h>
#include <math.h>
#include "lua_libs.h"
#include "../rage.h"
#include "../game_engine.h"
#include "../game_state.h"
#include "../mod/mod_manager.h"
#include "../mod/vehicletype.h"
#include "../mod/objecttype.h"
#include "../entity/vehicle.h"
#include "../entity/helicopter.h"
#include "../entity/object.h"
#include "../entity/unit.h"
#include "../entity/player.h"
#include "../entity/npc.h"

extern "C" {
	#include <lua.h>
	#include <lauxlib.h>
}

#include "LuaBridge/LuaBridge.h"


#define LUA_FUNC(name) static int name(lua_State *L)
#define LUA_REG(name) lua_register(L, #name, name)


Vehicle* addVehicle(string type, float x, float z)
{
	Vehicle *v;
	
	VehicleType *vt = GEng()->mm->getVehicleType(type);
	if (vt == NULL) {
		return NULL;
	}

	if (vt->helicopter) {
		v = new Helicopter(vt, getGameState(), x, z);
	} else {
		v = new Vehicle(vt, getGameState(), x, z);
	}
	
	getGameState()->addVehicle(v);
	
	return v;
}


/**
* Add a vehicle at the given coordinate
**/
Object* addObject(string type, float x, float z)
{
	Object *o;
	
	ObjectType *ot = GEng()->mm->getObjectType(type);
	if (ot == NULL) {
		return NULL;
	}
	
	o = new Object(ot, getGameState(), x, z, 1.0f, 0.0f);
	
	getGameState()->addObject(o);
	
	return o;
}


/**
* Loads the library into a lua state
*
* TODO: This is quite incomplete at the moment
**/
void load_entity_lib(lua_State *L)
{
	luabridge::getGlobalNamespace(L)
		.beginNamespace("world")
			.addFunction("addVehicle", &addVehicle)
			.addFunction("addObject", &addObject)
		.endNamespace()
		.beginNamespace("entity")
			
			// Entity
			.beginClass<Entity>("Entity")
				.addData("visible", &Entity::visible)
				.addFunction("delete", &Entity::hasDied)
			.endClass()
			
			// Entity : Vehicle
			.deriveClass<Vehicle, Entity>("Vehicle")
			.endClass()
			
			// Entity : Pickup
			.deriveClass<Pickup, Entity>("Pickup")
			.endClass()
			
			// Entity : Object
			.deriveClass<Object, Entity>("Object")
			.endClass()
			
			// Entity : Unit
			.beginClass<Unit>("Unit")
				.addData("faction", &Unit::fac)
				.addData("slot", &Unit::slot)
			.endClass()
			
			// Entity : Unit : Player
			.deriveClass<Player, Unit>("Player")
			.endClass()
			
			// Entity : Unit : NPC
			.deriveClass<NPC, Unit>("NPC")
			.endClass()
			
		.endNamespace();
}


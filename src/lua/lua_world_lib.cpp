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



/**
* Add a vehicle into the game world
**/
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
* Add an object at the given coordinate
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
* Methods which affect the game world directly, such as,
*  - Adding in entities at specific locations
*
* TODO: Getting lists of entities
* TODO: Other world stuff
* TODO: Should spawning be here or in the 'game' namespace?
**/
void load_world_lib(lua_State *L)
{
	luabridge::getGlobalNamespace(L)
	.beginNamespace("world")

		.addFunction("addVehicle", &addVehicle)
		.addFunction("addObject", &addObject)

	.endNamespace();
}


// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;


#include <stdio.h>
#include <math.h>
#include "lua_libs.h"
#include "../game_engine.h"
#include "../game_settings.h"
#include "../player_state.h"
#include "../mod/mod_manager.h"
#include "../mod/vehicletype.h"
#include "../mod/objecttype.h"
#include "../mod/pickuptype.h"
#include "../entity/vehicle.h"
#include "../entity/object.h"
#include "../entity/pickup.h"
#include "../entity/unit/unit.h"
#include "../entity/unit/player.h"
#include "../entity/unit/npc.h"
#include "../map/map.h"
#include "../map/zone.h"
#include "../fx/weather.h"

extern "C" {
	#include <lua.h>
	#include <lauxlib.h>
}

#include "LuaBridge/LuaBridge.h"



/**
* Return a randomly selected spawn zone
**/
Zone* getRandomSpawnZone(unsigned int fac)
{
	return getGameState()->map->getSpawnZone((Faction)fac);
}


/**
* Add a vehicle at a map X/Z coordinate
* Y will be calculated so it's on the ground
**/
Vehicle* addVehicleXZ(string type, float x, float z)
{
	Vehicle *v;

	VehicleType *vt = GEng()->mm->getVehicleType(type);
	if (vt == NULL) {
		return NULL;
	}

	v = new Vehicle(vt, getGameState(), x, z);
	getGameState()->addVehicle(v);
	return v;
}


/**
* Add a vehicle at a given coordinate
**/
Vehicle* addVehicleCoord(string type, btVector3 &coord)
{
	Vehicle *v;

	VehicleType *vt = GEng()->mm->getVehicleType(type);
	if (vt == NULL) {
		return NULL;
	}

	v = new Vehicle(vt, getGameState(), coord.x(), coord.y(), coord.z());
	getGameState()->addVehicle(v);
	return v;
}


/**
* Add a vehicle at a random location in a zone
**/
Vehicle* addVehicleZone(string type, Zone* zn)
{
	return addVehicleXZ(type, zn->getRandomX(), zn->getRandomZ());
}


/**
* Add an object at a given map X/Z coordinate
* The Y coordinate is calculated
**/
Object* addObjectXZ(string type, float x, float z)
{
	Object *o;

	ObjectType *ot = GEng()->mm->getObjectType(type);
	if (ot == NULL) {
		return NULL;
	}

	o = new Object(ot, getGameState(), x, z);

	getGameState()->addObject(o);

	return o;
}


/**
* Add an object at a given coordinate
**/
Object* addObjectCoord(string type, btVector3 &coord)
{
	Object *o;

	ObjectType *ot = GEng()->mm->getObjectType(type);
	if (ot == NULL) {
		return NULL;
	}

	o = new Object(ot, getGameState(), coord.x(), coord.y(), coord.z());

	getGameState()->addObject(o);

	return o;
}


/**
* Add an object at a random location on the map
**/
Object* addObjectRand(string type)
{
	return addObjectXZ(type, getGameState()->map->getRandomX(), getGameState()->map->getRandomZ());
}


/**
* Add an object at a random location in a zone
**/
Object* addObjectZone(string type, Zone* zn)
{
	return addObjectXZ(type, zn->getRandomX(), zn->getRandomZ());
}


/**
* Add an object at a given map X/Z coordinate
* The Y coordinate is calculated
**/
Pickup* addPickupXZ(string type, float x, float z)
{
	PickupType *pt = GEng()->mm->getPickupType(type);
	if (pt == NULL) {
		return NULL;
	}

	Pickup *p = new Pickup(pt, getGameState(), x, z);
	getGameState()->addPickup(p);

	return p;
}


/**
* Add an object at a given coordinate
**/
Pickup* addPickupCoord(string type, btVector3 &coord)
{
	PickupType *pt = GEng()->mm->getPickupType(type);
	if (pt == NULL) {
		return NULL;
	}

	Pickup *p = new Pickup(pt, getGameState(), coord.x(), coord.y(), coord.z());
	getGameState()->addPickup(p);

	return p;
}


/**
* Add an object at a random location on the map
**/
Pickup* addPickupRand(string type)
{
	return addPickupXZ(type, getGameState()->map->getRandomX(), getGameState()->map->getRandomZ());
}


/**
* Add an object at a random location in a zone
**/
Pickup* addPickupZone(string type, Zone* zn)
{
	return addPickupXZ(type, zn->getRandomX(), zn->getRandomZ());
}


/**
* Spawn in a player at a given map X/Z coordinate
**/
Player* addPlayerXZ(string type, unsigned int fac, unsigned int slot, float x, float z)
{
	Player *p;

	UnitType *ut = GEng()->mm->getUnitType(type);
	if (ut == NULL) {
		return NULL;
	}

	p = new Player(ut, getGameState(), (Faction)fac, slot, x, z);

	// Is it a local player?
	PlayerState *ps = getGameState()->getLocalPlayer(slot);
	if (ps != NULL) {
		ps->p = p;
	}

	getGameState()->addUnit(p);

	return p;
}


/**
* Spawn in a player at the given coordinate
**/
Player* addPlayerCoord(string type, unsigned int fac, unsigned int slot, btVector3 &coord)
{
	Player *p;

	UnitType *ut = GEng()->mm->getUnitType(type);
	if (ut == NULL) {
		return NULL;
	}

	p = new Player(ut, getGameState(), (Faction)fac, slot, coord.x(), coord.y(), coord.z());

	// Is it a local player?
	PlayerState *ps = getGameState()->getLocalPlayer(slot);
	if (ps != NULL) {
		ps->p = p;
	}

	getGameState()->addUnit(p);

	return p;
}


/**
* Spawn in a player in a random location of the specified zone
**/
Player* addPlayerZone(string type, unsigned int fac, unsigned int slot, Zone *zn)
{
	Player *p;

	UnitType *ut = GEng()->mm->getUnitType(type);
	if (ut == NULL) {
		return NULL;
	}

	p = new Player(ut, getGameState(), (Faction)fac, slot, zn->getRandomX(), zn->getRandomZ());

	// Is it a local player?
	PlayerState *ps = getGameState()->getLocalPlayer(slot);
	if (ps != NULL) {
		ps->p = p;
	}

	getGameState()->addUnit(p);

	return p;
}


/**
* Spawn in a player in a random location of a random spawn zone
**/
Player* addPlayer(string type, unsigned int fac, unsigned int slot)
{
	Zone *zn = getGameState()->map->getSpawnZone((Faction)fac);
	if (zn == NULL) {
		return NULL;
	}

	return addPlayerZone(type, fac, slot, zn);
}


/**
* Get a player from a given slot
**/
Player* getPlayerFromSlot(int slot)
{
	return (Player*) getGameState()->findUnitSlot(slot);
}


/**
* Spawn in a NPC at a given map X/Z coordinate
**/
NPC* addNpcXZ(string type, string aitype, unsigned int fac, float x, float z)
{
	NPC *p;

	UnitType *ut = GEng()->mm->getUnitType(type);
	if (ut == NULL) {
		return NULL;
	}

	AIType *ai = GEng()->mm->getAIType(aitype);
	if (ai == NULL) {
		return NULL;
	}

	p = new NPC(ut, getGameState(), (Faction)fac, ai, x, z);
	getGameState()->addUnit(p);

	return p;
}


/**
* Spawn in a NPC at the given coordinate
**/
NPC* addNpcCoord(string type, string aitype, unsigned int fac, btVector3 &coord)
{
	NPC *p;

	UnitType *ut = GEng()->mm->getUnitType(type);
	if (ut == NULL) {
		return NULL;
	}

	AIType *ai = GEng()->mm->getAIType(aitype);
	if (ai == NULL) {
		return NULL;
	}

	p = new NPC(ut, getGameState(), (Faction)fac, ai, coord.x(), coord.y(), coord.z());
	getGameState()->addUnit(p);

	return p;
}


/**
* Spawn in a NPC in a random location of the specified zone
**/
NPC* addNpcZone(string type, string aitype, unsigned int fac, Zone *zn)
{
	NPC *p;

	UnitType *ut = GEng()->mm->getUnitType(type);
	if (ut == NULL) {
		return NULL;
	}

	AIType *ai = GEng()->mm->getAIType(aitype);
	if (ai == NULL) {
		return NULL;
	}

	p = new NPC(ut, getGameState(), (Faction)fac, ai, zn->getRandomX(), zn->getRandomZ());
	getGameState()->addUnit(p);

	return p;
}


/**
* Spawn in a NPC in a random location of a random spawn zone
**/
NPC* addNpc(string type, string aitype, unsigned int fac)
{
	Zone *zn = getGameState()->map->getSpawnZone((Faction)fac);
	if (zn == NULL) {
		return NULL;
	}
	
	return addNpcZone(type, aitype, fac, zn);
}


/**
* Disable random weather
**/
void weatherDisableRandom()
{
	if (getGameState()->gs->gametype_weather) getGameState()->weather->disableRandom();
}


/**
* Enable random weather
**/
void weatherEnableRandom()
{
	if (getGameState()->gs->gametype_weather) getGameState()->weather->enableRandom();
}


/**
* Start or update rain
**/
void startRain(int flow)
{
	if (getGameState()->gs->gametype_weather) getGameState()->weather->startRain(flow);
}


/**
* Stop rain
**/
void stopRain()
{
	if (getGameState()->gs->gametype_weather) getGameState()->weather->stopRain();
}


/**
* Start or update rain
**/
void startSnow(int flow)
{
	if (getGameState()->gs->gametype_weather) getGameState()->weather->startSnow(flow);
}


/**
* Stop rain
**/
void stopSnow()
{
	if (getGameState()->gs->gametype_weather) getGameState()->weather->stopSnow();
}


void daynightDisableCycle()
{
	getGameState()->gs->day_night_cycle = false;
}

void daynightEnableCycle()
{
	getGameState()->gs->day_night_cycle = true;
}

void daynightSetMin(float min)
{
	if (min < getGameState()->gs->getTimeOfDayMin()) {
		min = getGameState()->gs->getTimeOfDayMin();
	}
	getGameState()->gs->tod_min = min;
}

void daynightSetMax(float max)
{
	if (max > getGameState()->gs->getTimeOfDayMin()) {
		max = getGameState()->gs->getTimeOfDayMin();
	}
	getGameState()->gs->tod_max = max;
}


void daynightAnimate(float target_val, float total_time)
{
	float diff = abs(getGameState()->time_of_day - target_val);
	getGameState()->tod_target_val = target_val;
	getGameState()->tod_target_psec = diff / total_time;
}


void daynightAnimateCancel()
{
	getGameState()->tod_target_val = 0.0f;
	getGameState()->tod_target_psec = 0.0f;
}



/**
* Methods which affect the game world such as,
*  - Adding in entities at specific locations
*  - TODO: Getting lists of entities
**/
void load_world_lib(lua_State *L)
{
	luabridge::getGlobalNamespace(L)
	.beginNamespace("game")

		.beginClass<Zone>("Zone")
			.addFunction("getRandomX", &Zone::getRandomX)
			.addFunction("getRandomZ", &Zone::getRandomZ)
		.endClass()

		.addFunction("getRandomSpawnZone", &getRandomSpawnZone)

		.addFunction("addVehicleXZ", &addVehicleXZ)
		.addFunction("addVehicleCoord", &addVehicleCoord)
		.addFunction("addVehicleZone", &addVehicleZone)

		.addFunction("addObjectXZ", &addObjectXZ)
		.addFunction("addObjectCoord", &addObjectCoord)
		.addFunction("addObjectRand", &addObjectRand)
		.addFunction("addObjectZone", &addObjectZone)

		.addFunction("addPickupXZ", &addPickupXZ)
		.addFunction("addPickupCoord", &addPickupCoord)
		.addFunction("addPickupRand", &addPickupRand)
		.addFunction("addPickupZone", &addPickupZone)
		
		.addFunction("addPlayer", &addPlayer)
		.addFunction("addPlayerXZ", &addPlayerXZ)
		.addFunction("addPlayerCoord", &addPlayerCoord)
		.addFunction("addPlayerZone", &addPlayerZone)

		.addFunction("getPlayerFromSlot", &getPlayerFromSlot)

		.addFunction("addNpc", &addNpc)
		.addFunction("addNpcXZ", &addNpcXZ)
		.addFunction("addNpcCoord", &addNpcCoord)
		.addFunction("addNpcZone", &addNpcZone)

	.endNamespace()
	.beginNamespace("weather")

		.addFunction("disableRandom", &weatherDisableRandom)
		.addFunction("enableRandom", &weatherEnableRandom)
		.addFunction("startRain", &startRain)
		.addFunction("stopRain", &stopRain)
		.addFunction("startSnow", &startSnow)
		.addFunction("stopSnow", &stopSnow)

	.endNamespace()
	.beginNamespace("daynight")

		.addFunction("disableCycle", &daynightDisableCycle)
		.addFunction("enableCycle", &daynightEnableCycle)
		.addFunction("setMin", &daynightSetMin)
		.addFunction("setMax", &daynightSetMax)
		.addFunction("animate", &daynightAnimate)
		.addFunction("animateCancel", &daynightAnimateCancel)

	.endNamespace();
}

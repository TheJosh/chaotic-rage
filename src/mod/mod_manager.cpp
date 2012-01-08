// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <string>
#include <SDL.h>
#include <confuse.h>
#include <zzip/zzip.h>
#include "../rage.h"

using namespace std;



/**
* Inits the mod-manager.
**/
ModManager::ModManager(GameState * st)
{
	this->st = st;
	st->mm = this;

	this->mods = new vector<Mod*>();
}



/**
* Add a mod to the list
**/
void ModManager::addMod(Mod * mod)
{
	this->mods->push_back(mod);
}

/**
* Get a mod via name
**/
Mod * ModManager::getMod(string name)
{
	if (name.empty()) return NULL;
	
	int i;
	for (i = this->mods->size() - 1; i >= 0; --i) {
		if (this->mods->at(i)->name.compare(name) == 0) return this->mods->at(i);
	}
	
	reportFatalError("Data module is not loaded: " + name);
	return NULL;
}

/**
* Get the 'default' mod
**/
Mod * ModManager::getDefaultMod()
{
	return this->mods->at(0);
}

/**
* Reload the attrs for all mods.
* Development/Debugging feature only.
*
* TODO: Should actually operate on all mods, currently only default mod
**/
bool ModManager::reloadAttrs()
{
	return this->getDefaultMod()->reloadAttrs();
}


/**
* Gets an animmodel by ID
**/
AnimModel * ModManager::getAnimModel(int id)
{
	return this->getDefaultMod()->getAnimModel(id);
}

/**
* Gets an animmodel by name
**/
AnimModel * ModManager::getAnimModel(string name)
{
	return this->getDefaultMod()->getAnimModel(name);
}



/**
* Gets an floor type by ID
**/
FloorType * ModManager::getFloorType(int id)
{
	return this->getDefaultMod()->getFloorType(id);
}

/**
* Gets an floor type by name
**/
FloorType * ModManager::getFloorType(string name)
{
	return this->getDefaultMod()->getFloorType(name);
}



/**
* Gets a gametype by ID
**/
GameType * ModManager::getGameType(int id)
{
	return this->getDefaultMod()->getGameType(id);
}

/**
* Gets a gametype by name
**/
GameType * ModManager::getGameType(string name)
{
	return this->getDefaultMod()->getGameType(name);
}

/**
* Returns a start and end iterator for getting all gametypes (default mod only atm)
**/
void ModManager::getAllGameTypes(vector<GameType*>::iterator * start, vector<GameType*>::iterator * end)
{
	this->getDefaultMod()->getAllGameTypes(start, end);
}



/**
* Gets a object type by ID
**/
ObjectType * ModManager::getObjectType(int id)
{
	return this->getDefaultMod()->getObjectType(id);
}

/**
* Gets a object type by name
**/
ObjectType * ModManager::getObjectType(string name)
{
	return this->getDefaultMod()->getObjectType(name);
}

/**
* Adds a dynamically-created object type to the default mod
**/
void ModManager::addObjectType(ObjectType * ot)
{
	this->getDefaultMod()->addObjectType(ot);
}



/**
* Gets a particle type by ID
**/
ParticleType * ModManager::getParticleType(int id)
{
	return this->getDefaultMod()->getParticleType(id);
}

/**
* Gets a particle type by name
**/
ParticleType * ModManager::getParticleType(string name)
{
	return this->getDefaultMod()->getParticleType(name);
}



/**
* Gets a particle generator by ID
**/
ParticleGenType * ModManager::getParticleGenType(int id)
{
	return this->getDefaultMod()->getParticleGenType(id);
}

/**
* Gets a particle generator by name
**/
ParticleGenType * ModManager::getParticleGenType(string name)
{
	return this->getDefaultMod()->getParticleGenType(name);
}



/**
* Gets a unit type by ID
**/
UnitType * ModManager::getUnitType(int id)
{
	return this->getDefaultMod()->getUnitType(id);
}

/**
* Gets a unit type by name
**/
UnitType * ModManager::getUnitType(string name)
{
	return this->getDefaultMod()->getUnitType(name);
}

/**
* Returns a start and end iterator for getting all unit types (default mod only atm)
**/
void ModManager::getAllUnitTypes(vector<UnitType*>::iterator * start, vector<UnitType*>::iterator * end)
{
	this->getDefaultMod()->getAllUnitTypes(start, end);
}



/**
* Gets a song by ID
**/
Song * ModManager::getSong(int id)
{
	return this->getDefaultMod()->getSong(id);
}

/**
* Gets a song by name
**/
Song * ModManager::getSong(string name)
{
	return this->getDefaultMod()->getSong(name);
}

/**
* Gets a random song
**/
Song * ModManager::getRandomSong()
{
	return this->getDefaultMod()->getRandomSong();
}



/**
* Gets a sound by ID
**/
Sound * ModManager::getSound(int id)
{
	return this->getDefaultMod()->getSound(id);
}

/**
* Gets a sound by name
**/
Sound * ModManager::getSound(string name)
{
	return this->getDefaultMod()->getSound(name);
}



/**
* Gets a wall type by ID
**/
WallType * ModManager::getWallType(int id)
{
	return this->getDefaultMod()->getWallType(id);
}

/**
* Gets a wall type by name
**/
WallType * ModManager::getWallType(string name)
{
	if (name.empty()) return NULL;
	
	for (int i = 0; i <= this->mods->size(); i++) {
		Mod *mod = this->mods->at(i);
		WallType *wt = mod->getWallType(name);
		if (wt) return wt;
	}
	
	return NULL;
}



/**
* Gets a weapon type by ID
**/
WeaponType * ModManager::getWeaponType(int id)
{
	return this->getDefaultMod()->getWeaponType(id);
}

/**
* Gets a weapon type by name
**/
WeaponType * ModManager::getWeaponType(string name)
{
	return this->getDefaultMod()->getWeaponType(name);
}




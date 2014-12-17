// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <string>
#include <algorithm>
#include <confuse.h>
#include "../rage.h"
#include "../game_state.h"
#include "../game_engine.h"
#include "../util/ui_update.h"
#include "../util/cmdline.h"
#include "../render_opengl/assimpmodel.h"
#include "../render/render_3d.h"
#include "mod.h"
#include "mod_manager.h"
#include "confuse_types.h"
#include "aitype.h"
#include "campaign.h"
#include "objecttype.h"
#include "unittype.h"
#include "song.h"
#include "walltype.h"
#include "vehicletype.h"
#include "weapontype.h"
#include "gametype.h"



using namespace std;



/**
* Load the main mod and user mods.
* Run while the intro is being displayed, on a background thread.
**/
bool loadMods(GameState *st, UIUpdate* ui)
{
	// Load main mod
	Mod * mod = new Mod(st, "data/cr");
	if (! mod->load(ui)) {
		reportFatalError("Unable to load mod 'cr'.");
	}
	GEng()->mm->addMod(mod);
	GEng()->mm->setBase(mod);

	// Load the game font from the main mod
	if (GEng()->render->is3D()) {
		static_cast<Render3D*>(GEng()->render)->loadFont("DejaVuSans.ttf", mod);
	}

	// If a suppl mod has been specified on the cmdline, try to load it
	if (GEng()->cmdline->mod != "" && GEng()->cmdline->mod != mod->name) {
		Mod* newsuppl = new Mod(st, "data/" + GEng()->cmdline->mod);
		if (! newsuppl->load(ui)) {
			reportFatalError("Unable to load mod '" + GEng()->cmdline->mod + "'.");
		}
		GEng()->mm->addMod(newsuppl);
		GEng()->mm->setSuppl(newsuppl);
	}

	// Load user mods
	vector<string> * userfiles = getUserModFilenames();
	for (unsigned int i = 0; i < userfiles->size(); i++) {
		mod = new Mod(st, userfiles->at(i));
		if (! mod->load(ui)) {
			reportFatalError("Unable to load user mod '" + userfiles->at(i) + "'.");
		}
		GEng()->mm->addMod(mod);
	}
	delete(userfiles);

	if (ui) ui->updateUI();

	return true;
}



/**
* Inits the mod-manager.
**/
ModManager::ModManager()
{
	this->mods = new vector<Mod*>();
	this->base = NULL;
	this->suppl = NULL;
}


ModManager::~ModManager()
{
	for (int i = this->mods->size() - 1; i >= 0; --i) {
		delete(this->mods->at(i));
	}
	delete(this->mods);
}


/**
* Add a mod to the list
**/
void ModManager::addMod(Mod * mod)
{
	this->mods->push_back(mod);
}


/**
* Remove a mod from the list, and nuke it too
**/
void ModManager::remMod(Mod *mod)
{
	if (mod == NULL) return;
	if (mod == this->suppl) this->suppl = NULL;
	this->mods->erase(std::remove(this->mods->begin(), this->mods->end(), mod), this->mods->end());
	delete(mod);
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
* If there is a suppl mod, return it.
* Otherwise return the base mod.
**/
Mod * ModManager::getSupplOrBase()
{
	return (this->suppl ? this->suppl : this->base);
}


/**
* Get a list of all loaded mods
* Don't forget to delete() the result
**/
vector<string> * ModManager::getLoadedMods()
{
	vector<string> * out = new vector<string>();
	for (unsigned int i = 0; i < this->mods->size(); i++) {
		Mod *mod = this->mods->at(i);
		out->push_back(mod->name);
	}
	return out;
}


/**
* Get a list of all available mods
* Don't forget to delete() the result
**/
vector<string> * ModManager::getAvailMods()
{
	vector<string>* mods = getSystemModNames();
	vector<string>* out = new vector<string>();
	out->push_back("cr");	// force to top of list
	for (vector<string>::iterator it = mods->begin(); it != mods->end(); ++it) {
		if ((*it) != "debug" && (*it) != "intro" && (*it) != "cr") {
			out->push_back(*it);
		}
	}
	delete(mods);
	return out;
}


/**
* Reload the attrs for all mods.
* Development/Debugging feature only.
**/
bool ModManager::reloadAttrs()
{
	for (unsigned int i = 0; i < this->mods->size(); i++) {
		Mod *mod = this->mods->at(i);
		if (! mod->reloadAttrs()) return false;
	}
	return true;
}



/***********   GET BY ID   ***********/

/**
* Gets an AI type by ID
**/
AIType * ModManager::getAIType(CRC32 id)
{
	for (unsigned int i = 0; i < this->mods->size(); i++) {
		Mod *mod = this->mods->at(i);
		AIType *et = mod->getAIType(id);
		if (et) return et;
	}

	return NULL;
}


/**
* Gets a object type by ID
**/
ObjectType * ModManager::getObjectType(CRC32 id)
{
	for (unsigned int i = 0; i < this->mods->size(); i++) {
		Mod *mod = this->mods->at(i);
		ObjectType *et = mod->getObjectType(id);
		if (et) return et;
	}

	return NULL;
}


/**
* Gets a pickup type by ID
**/
PickupType * ModManager::getPickupType(CRC32 id)
{
	for (unsigned int i = 0; i < this->mods->size(); i++) {
		Mod *mod = this->mods->at(i);
		PickupType *et = mod->getPickupType(id);
		if (et) return et;
	}

	return NULL;
}


/**
* Gets a unit type by ID
**/
UnitType * ModManager::getUnitType(CRC32 id)
{
	for (unsigned int i = 0; i < this->mods->size(); i++) {
		Mod *mod = this->mods->at(i);
		UnitType *et = mod->getUnitType(id);
		if (et) return et;
	}

	return NULL;
}


/**
* Gets an vehicle type by ID
**/
VehicleType * ModManager::getVehicleType(CRC32 id)
{
	for (unsigned int i = 0; i < this->mods->size(); i++) {
		Mod *mod = this->mods->at(i);
		VehicleType *et = mod->getVehicleType(id);
		if (et) return et;
	}

	return NULL;
}


/**
* Gets a wall type by ID
**/
WallType * ModManager::getWallType(CRC32 id)
{
	for (unsigned int i = 0; i < this->mods->size(); i++) {
		Mod *mod = this->mods->at(i);
		WallType *et = mod->getWallType(id);
		if (et) return et;
	}

	return NULL;
}


/**
* Gets a weapon type by ID
**/
WeaponType * ModManager::getWeaponType(CRC32 id)
{
	for (unsigned int i = 0; i < this->mods->size(); i++) {
		Mod *mod = this->mods->at(i);
		WeaponType *et = mod->getWeaponType(id);
		if (et) return et;
	}

	return NULL;
}



/***********   GET BY NAME   ***********/

/**
* Gets an animmodel by name
**/
AIType * ModManager::getAIType(string name)
{
	if (name.empty()) return NULL;

	for (unsigned int i = 0; i < this->mods->size(); i++) {
		Mod *mod = this->mods->at(i);
		AIType *et = mod->getAIType(name);
		if (et) return et;
	}

	return NULL;
}


/**
* Gets an vehicle type by name
**/
VehicleType * ModManager::getVehicleType(string name)
{
	if (name.empty()) return NULL;

	for (unsigned int i = 0; i < this->mods->size(); i++) {
		Mod *mod = this->mods->at(i);
		VehicleType *et = mod->getVehicleType(name);
		if (et) return et;
	}

	return NULL;
}


/**
* Gets a gametype by name
**/
GameType * ModManager::getGameType(string name)
{
	if (name.empty()) return NULL;

	for (unsigned int i = 0; i < this->mods->size(); i++) {
		Mod *mod = this->mods->at(i);
		GameType *et = mod->getGameType(name);
		if (et) return et;
	}

	return NULL;
}


/**
* Used for sorting
**/
struct GameTypesPointerCompare {
	bool operator()(const GameType* l, const GameType* r) {
		return *l < *r;
	}
};

/**
* Returns a start and end iterator for getting all gametypes
* Don't forget to `delete` when you are done.
**/
vector<GameType*> * ModManager::getAllGameTypes()
{
	vector<GameType*>::iterator start, end;
	vector<GameType*> * gt;

	gt = new vector<GameType*>();

	for (unsigned int i = 0; i < this->mods->size(); i++) {
		Mod *mod = this->mods->at(i);
		mod->getAllGameTypes(&start, &end);
		gt->insert(gt->end(), start, end);
	}

	std::sort(gt->begin(), gt->end(), GameTypesPointerCompare());

	return gt;
}


/**
* Gets a object type by name
**/
ObjectType * ModManager::getObjectType(string name)
{
	if (name.empty()) return NULL;

	for (unsigned int i = 0; i < this->mods->size(); i++) {
		Mod *mod = this->mods->at(i);
		ObjectType *et = mod->getObjectType(name);
		if (et) return et;
	}

	return NULL;
}


/**
* Gets a pickup type by name
**/
PickupType * ModManager::getPickupType(string name)
{
	if (name.empty()) return NULL;

	for (unsigned int i = 0; i < this->mods->size(); i++) {
		Mod *mod = this->mods->at(i);
		PickupType *et = mod->getPickupType(name);
		if (et) return et;
	}

	return NULL;
}


/**
* Gets a unit type by name
**/
UnitType * ModManager::getUnitType(string name)
{
	if (name.empty()) return NULL;

	for (unsigned int i = 0; i < this->mods->size(); i++) {
		Mod *mod = this->mods->at(i);
		UnitType *et = mod->getUnitType(name);
		if (et) return et;
	}

	return NULL;
}

/**
* Returns a start and end iterator for getting all unit types
* Don't forget to `delete` when you are done.
**/
vector<UnitType*> * ModManager::getAllUnitTypes()
{
	vector<UnitType*>::iterator start, end;
	vector<UnitType*> * ut;

	ut = new vector<UnitType*>();

	for (unsigned int i = 0; i < this->mods->size(); i++) {
		Mod *mod = this->mods->at(i);
		mod->getAllUnitTypes(&start, &end);
		ut->insert(ut->end(), start, end);
	}

	return ut;
}


/**
* Gets a song by name
**/
Song * ModManager::getSong(string name)
{
	if (name.empty()) return NULL;

	for (unsigned int i = 0; i < this->mods->size(); i++) {
		Mod *mod = this->mods->at(i);
		Song *et = mod->getSong(name);
		if (et) return et;
	}

	return NULL;
}


/**
* Gets a random song
**/
Song * ModManager::getRandomSong()
{
	Mod *mod = this->mods->at(getRandom(0, mods->size() - 1));
	return mod->getRandomSong();
}


/**
* Gets a wall type by name
**/
WallType * ModManager::getWallType(string name)
{
	if (name.empty()) return NULL;

	for (unsigned int i = 0; i < this->mods->size(); i++) {
		Mod *mod = this->mods->at(i);
		WallType *et = mod->getWallType(name);
		if (et) return et;
	}

	return NULL;
}


/**
* Gets a weapon type by name
**/
WeaponType * ModManager::getWeaponType(string name)
{
	if (name.empty()) return NULL;

	for (unsigned int i = 0; i < this->mods->size(); i++) {
		Mod *mod = this->mods->at(i);
		WeaponType *et = mod->getWeaponType(name);
		if (et) return et;
	}

	return NULL;
}


/**
* Returns a start and end iterator for getting all weapon types
* Don't forget to `delete` when you are done.
**/
vector<WeaponType*> * ModManager::getAllWeaponTypes()
{
	vector<WeaponType*>::iterator start, end;
	vector<WeaponType*> * wts;

	wts = new vector<WeaponType*>();

	for (unsigned int i = 0; i < this->mods->size(); i++) {
		Mod *mod = this->mods->at(i);
		mod->getAllWeaponTypes(&start, &end);
		wts->insert(wts->end(), start, end);
	}

	return wts;
}



// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include <confuse.h>
#include <zzip/zzip.h>
#include "rage.h"

using namespace std;


/**
* Inits the mod. Does not load yet
**/
Mod::Mod(GameState * st, string directory)
{
	this->st = st;
	st->addMod(this);
	
	this->directory = directory;
}


/**
* Loads the mod
**/
bool Mod::load()
{
	int i;
	
	animmodels = loadAllAnimModels(this);
	if (animmodels == NULL) return false;
	
	for (i = animmodels->size() - 1; i >= 0; --i) {
		animmodels->at(i)->next = this->getAnimModel(animmodels->at(i)->next_name);
	}
	
	areatypes = loadAllAreaTypes(this);
	if (areatypes == NULL) return false;
	
	particletypes = loadAllParticleTypes(this);
	if (particletypes == NULL) return false;
	
	pgeneratortypes = loadAllParticleGenTypes(this);
	if (pgeneratortypes == NULL) return false;
	
	unitclasses = loadAllUnitClasses(this);
	if (unitclasses == NULL) return false;
	
	songs = loadAllSongs(this);
	if (songs == NULL) return false;
	
	walltypes = loadAllWallTypes(this);
	if (walltypes == NULL) return false;
	
	weapontypes = loadAllWeaponTypes(this);
	if (weapontypes == NULL) return false;
	
	return true;
}


/**
* Gets an animmodel by ID
**/
AnimModel * Mod::getAnimModel(int id)
{
	if (id < 0 or ((unsigned int) id) > animmodels->size()) return NULL;
	return animmodels->at(id);
}

AnimModel * Mod::getAnimModel(string name)
{
	if (name == "") return NULL;
	
	int i;
	for (i = animmodels->size() - 1; i >= 0; --i) {
		if (animmodels->at(i)->name == name) return animmodels->at(i);
	}
	return NULL;
}

/**
* Gets an areatype by ID
**/
AreaType * Mod::getAreaType(int id)
{
	if (id < 0 or ((unsigned int) id) > areatypes->size()) return NULL;
	return areatypes->at(id);
}

/**
* Gets a particle type by ID
**/
ParticleType * Mod::getParticleType(int id)
{
	if (id < 0 or ((unsigned int) id) > particletypes->size()) return NULL;
	return particletypes->at(id);
}

/**
* Gets a particle generator by ID
**/
ParticleGenType * Mod::getParticleGenType(int id)
{
	if (id < 0 or ((unsigned int) id) > pgeneratortypes->size()) return NULL;
	return pgeneratortypes->at(id);
}

/**
* Gets a unitclass by ID
**/
UnitClass * Mod::getUnitClass(int id)
{
	if (id < 0 or ((unsigned int) id) > unitclasses->size()) return NULL;
	return unitclasses->at(id);
}

/**
* Gets a song by ID
**/
Song * Mod::getSong(int id)
{
	if (id < 0 or ((unsigned int) id) > songs->size()) return NULL;
	return songs->at(id);
}

/**
* Gets a wall type by ID
**/
WallType * Mod::getWallType(int id)
{
	if (id < 0 or ((unsigned int) id) > walltypes->size()) return NULL;
	return walltypes->at(id);
}

/**
* Gets a weapon type by ID
**/
WeaponType * Mod::getWeaponType(int id)
{
	if (id < 0 or ((unsigned int) id) > weapontypes->size()) return NULL;
	return weapontypes->at(id);
}



/**
* Loads a text file into a char * pointer.
* Don't forget to free() when you are done.
**/
char * Mod::loadText(string resname)
{
	ZZIP_FILE *fp;
	char *buffer;
	
	string filename = directory;
	filename.append(resname);
	
	fp = zzip_open(filename.c_str(), 0);
	if (! fp) {
		fprintf(stderr, "Couldn't load resource '%s'.\n", resname.c_str());
		return NULL;
	}
	
	// Read the contents of the file into a buffer
	zzip_seek (fp, 0, SEEK_END);
	int len = zzip_tell (fp);
	zzip_seek (fp, 0, SEEK_SET);
	
	buffer = (char*) malloc(len + 1);
	if (buffer == NULL) {
		fprintf(stderr, "Couldn't load resource '%s'.\n", resname.c_str());
		return NULL;
	}
	buffer[len] = '\0';
	
	zzip_read(fp, buffer, len);
	zzip_close(fp);
	
	return buffer;
}


/**
* Loads a resource into a SDL_RWops.
* Don't forget to SDL_RWclose() when you are done.
**/
SDL_RWops * Mod::loadRWops(string resname)
{
	SDL_RWops *rw;
	
	string filename = directory;
	filename.append(resname);
	
	rw = SDL_RWFromZZIP(filename.c_str(), "rb");
	if (rw == NULL) {
		fprintf(stderr, "Couldn't load resource '%s'.\n", resname.c_str());
		return NULL;
	}
	
	return rw;
}




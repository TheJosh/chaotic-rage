// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <string>
#include <SDL.h>
#include <confuse.h>
#include <zzip/zzip.h>
#include "rage.h"

using namespace std;


/**
* Inits the mod. Does not load yet.
* Directory should NOT contain a trailing slash
**/
Mod::Mod(GameState * st, string directory)
{
	this->st = st;
	st->addMod(this);
	
	int bp = directory.rfind('/');
	this->name = directory.substr(bp + 1, 100);
	
	directory.append("/");
	this->directory = directory;
}


/**
* This function loads a mod from a file into a vector of a specified class.
*
* It's a templated function, so it needs type T to be specified in the function call.
* e.g.
*   loadModFile<AnimModel*>(...) to load into the AnimModel class
*
* Params:
*   mod         The mod which is loading the data
*   filename    The .conf filename to load data from, relative to the mod directory
*   section     The section name to load from the file
*   item_opts   The libconfuse option definition for the item
*   item_f      Function pointer for the processing function for the item
*
*               This should have the signature:
*                 T* loadItem(cfg_t* cfg_item, Mod* mod);
*
*               Example:
*                 AnimModel* loadItemAnimModel(cfg_t* cfg_item, Mod* mod);
**/
template <class T>
vector<T> * loadModFile(Mod* mod, const char* filename, const char* section, cfg_opt_t* item_opts, T (*item_f)(cfg_t*, Mod*))
{
	vector<T> * models = new vector<T>();
	
	char *buffer;
	cfg_t *cfg, *cfg_item;
	
	cfg_opt_t opts[] =
	{
		CFG_SEC((char*) section, item_opts, CFGF_MULTI),
		CFG_END()
	};
	
	// Load + parse the config file
	buffer = mod->loadText(filename);
	if (buffer == NULL) {
		cerr << "Error loading file " << filename << ". File is empty or missing.\n";
		return NULL;
	}
	
	cfg = cfg_init(opts, CFGF_NONE);
	cfg_parse_buf(cfg, buffer);
	
	free(buffer);
	
	
	int num_types = cfg_size(cfg, section);
	if (num_types == 0) {
		cerr << "Error loading file " << filename << ". File does not contain any '" << section << "' sections.\n";
		return NULL;
	}

	// Process area type sections
	int j;
	for (j = 0; j < num_types; j++) {
		cfg_item = cfg_getnsec(cfg, section, j);
		
		T am = (*item_f)(cfg_item, mod);
		if (am == NULL) {
			cerr << "Bad definition in file " << filename << " at index " << j << "\n";
			return NULL;
		}
		
		models->push_back(am);
		am->id = models->size() - 1;
	}
	
	return models;
}



/**
* Loads the mod
**/
bool Mod::load()
{
	animmodels = loadModFile<AnimModel*>(this, "animmodels.conf", "animmodel", animmodel_opts, &loadItemAnimModel);
	if (animmodels == NULL) return false;
		
	sounds = loadModFile<Sound*>(this, "sounds.conf", "sound", sound_opts, &loadItemSound);
	if (sounds == NULL) return false;
	
	songs = loadModFile<Song*>(this, "songs.conf", "song", song_opts, &loadItemSong);
	if (songs == NULL) return false;


	areatypes = loadModFile<FloorType*>(this, "floortypes.conf", "floortype", floortype_opts, &loadItemFloorType);
	if (areatypes == NULL) return false;
	
	objecttypes = loadModFile<ObjectType*>(this, "objecttypes.conf", "objecttype", objecttype_opts, &loadItemObjectType);
	if (objecttypes == NULL) return false;

	particletypes = loadModFile<ParticleType*>(this, "particletypes.conf", "particle", particletype_opts, &loadItemParticleType);
	if (particletypes == NULL) return false;
	
	pgeneratortypes = loadModFile<ParticleGenType*>(this, "particlegenerators.conf", "generator", generatortype_opts, &loadItemParticleGenType);
	if (pgeneratortypes == NULL) return false;
	
	walltypes = loadModFile<WallType*>(this, "walltypes.conf", "walltype", walltype_opts, &loadItemWallType);
	if (walltypes == NULL) return false;
	
	weapontypes = loadModFile<WeaponType*>(this, "weapontypes.conf", "weapon", weapontype_opts, &loadItemWeaponType);
	if (weapontypes == NULL) return false;
	
	unitclasses = loadModFile<UnitType*>(this, "unittypes.conf", "unittype", unittype_opts, &loadItemUnitType);
	if (unitclasses == NULL) return false;
	
	
	gametypes = loadModFile<GameType*>(this, "gametypes.conf", "gametype", gametype_opts, &loadItemGameType);
	if (gametypes == NULL) return false;
	
	
	// Post-load logic
	for (int i = animmodels->size() - 1; i >= 0; --i) {
		animmodels->at(i)->next = this->getAnimModel(animmodels->at(i)->next_name);
	}
	
	return true;
}


/**
* Reloads the attributes of some of the key config files
**/
bool Mod::reloadAttrs()
{
	int i;
	
	// Weapons
	vector<WeaponType*> * n_weapontypes = loadModFile<WeaponType*>(this, "weapontypes.conf", "weapon", weapontype_opts, &loadItemWeaponType);
	if (n_weapontypes == NULL) return false;
	
	for (i = n_weapontypes->size() - 1; i >= 0; --i) {
		WeaponType *nu = n_weapontypes->at(i);
		WeaponType *old = this->getWeaponType(nu->name);
		if (! old) continue;
		
		old->pt = nu->pt;
		old->title = nu->title;
		old->angle_range = nu->angle_range;
		old->rate = nu->rate;
		old->continuous = nu->continuous;
		old->magazine_limit = nu->magazine_limit;
		old->belt_limit = nu->belt_limit;
	}
	
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
	if (name.empty()) return NULL;
	
	int i;
	for (i = animmodels->size() - 1; i >= 0; --i) {
		if (animmodels->at(i)->name.compare(name) == 0) return animmodels->at(i);
	}
	return NULL;
}

/**
* Gets an areatype by ID
**/
FloorType * Mod::getFloorType(int id)
{
	if (id < 0 or ((unsigned int) id) > areatypes->size()) return NULL;
	return areatypes->at(id);
}


/**
* Gets a gametype by ID
**/
GameType * Mod::getGameType(int id)
{
	if (id < 0 or ((unsigned int) id) > gametypes->size()) return NULL;
	return gametypes->at(id);
}

GameType * Mod::getGameType(string name)
{
	if (name.empty()) return NULL;
	
	int i;
	for (i = gametypes->size() - 1; i >= 0; --i) {
		if (gametypes->at(i)->name.compare(name) == 0) return gametypes->at(i);
	}
	return NULL;
}

/**
* Returns two iterators for getting all gametypes
**/
void Mod::getAllGameTypes(vector<GameType*>::iterator * start, vector<GameType*>::iterator * end)
{
	*start = gametypes->begin();
	*end = gametypes->end();
}



/**
* Gets a unitclass by ID
**/
ObjectType * Mod::getObjectType(int id)
{
	if (id < 0 or ((unsigned int) id) > objecttypes->size()) return NULL;
	return objecttypes->at(id);
}

/**
* Gets a unitclass by name
**/
ObjectType * Mod::getObjectType(string name)
{
	if (name.empty()) return NULL;
	
	int i;
	for (i = objecttypes->size() - 1; i >= 0; --i) {
		if (objecttypes->at(i)->name.compare(name) == 0) return objecttypes->at(i);
	}
	return NULL;
}


/**
* Gets a particle type by ID
**/
ParticleType * Mod::getParticleType(int id)
{
	if (id < 0 or ((unsigned int) id) > particletypes->size()) return NULL;
	return particletypes->at(id);
}

ParticleType * Mod::getParticleType(string name)
{
	if (name.empty()) return NULL;
	
	int i;
	for (i = particletypes->size() - 1; i >= 0; --i) {
		if (particletypes->at(i)->name.compare(name) == 0) return particletypes->at(i);
	}
	return NULL;
}


/**
* Gets a particle generator by ID
**/
ParticleGenType * Mod::getParticleGenType(int id)
{
	if (id < 0 or ((unsigned int) id) > pgeneratortypes->size()) return NULL;
	return pgeneratortypes->at(id);
}

ParticleGenType * Mod::getParticleGenType(string name)
{
	if (name.empty()) return NULL;
	
	int i;
	for (i = pgeneratortypes->size() - 1; i >= 0; --i) {
		if (pgeneratortypes->at(i)->name.compare(name) == 0) return pgeneratortypes->at(i);
	}
	return NULL;
}


/**
* Gets a unitclass by ID
**/
UnitType * Mod::getUnitType(int id)
{
	if (id < 0 or ((unsigned int) id) > unitclasses->size()) return NULL;
	return unitclasses->at(id);
}

/**
* Gets a unitclass by name
**/
UnitType * Mod::getUnitType(string name)
{
	if (name.empty()) return NULL;
	
	int i;
	for (i = unitclasses->size() - 1; i >= 0; --i) {
		if (unitclasses->at(i)->name.compare(name) == 0) return unitclasses->at(i);
	}
	return NULL;
}


/**
* Returns two iterators for getting all gametypes
**/
void Mod::getAllUnitTypes(vector<UnitType*>::iterator * start, vector<UnitType*>::iterator * end)
{
	*start = unitclasses->begin();
	*end = unitclasses->end();
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
* Gets a random song
**/
Song * Mod::getRandomSong()
{
	return songs->at(getRandom(0, songs->size() - 1));
}


/**
* Gets a sound by ID
**/
Sound * Mod::getSound(int id)
{
	if (id < 0 or ((unsigned int) id) > sounds->size()) return NULL;
	return sounds->at(id);
}

Sound * Mod::getSound(string name)
{
	if (name.empty()) return NULL;
	
	int i;
	for (i = sounds->size() - 1; i >= 0; --i) {
		if (sounds->at(i)->name.compare(name) == 0) return sounds->at(i);
	}
	return NULL;
}


/**
* Gets a wall type by ID
**/
WallType * Mod::getWallType(int id)
{
	if (id < 0 or ((unsigned int) id) > walltypes->size()) return NULL;
	return walltypes->at(id);
}

WallType * Mod::getWallType(string name)
{
	if (name.empty()) return NULL;
	
	int i;
	for (i = walltypes->size() - 1; i >= 0; --i) {
		if (walltypes->at(i)->name.compare(name) == 0) return walltypes->at(i);
	}
	return NULL;
}


/**
* Gets a weapon type by ID
**/
WeaponType * Mod::getWeaponType(int id)
{
	if (id < 0 or ((unsigned int) id) > weapontypes->size()) return NULL;
	return weapontypes->at(id);
}

WeaponType * Mod::getWeaponType(string name)
{
	if (name.empty()) return NULL;
	
	int i;
	for (i = weapontypes->size() - 1; i >= 0; --i) {
		if (weapontypes->at(i)->name.compare(name) == 0) return weapontypes->at(i);
	}
	return NULL;
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
		cerr << "Couldn't load resource '" << resname << "'. Unable to locate resource.\n";
		return NULL;
	}
	
	// Read the contents of the file into a buffer
	zzip_seek (fp, 0, SEEK_END);
	int len = zzip_tell (fp);
	zzip_seek (fp, 0, SEEK_SET);
	
	buffer = (char*) malloc(len + 1);
	if (buffer == NULL) {
		cerr << "Couldn't load resource '" << resname << "'. Unable to allocate memory.\n";
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




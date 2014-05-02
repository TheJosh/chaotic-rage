// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <string>
#include <map>
#include <SDL.h>
#include <confuse.h>
#include "../rage.h"
#include "../game_state.h"
#include "../game_engine.h"
#include "../util/ui_update.h"
#include "../render_opengl/assimpmodel.h"
#include "../util/crc32.h"
#include "../util/debug.h"
#include "mod.h"
#include "confuse_types.h"
#include "aitype.h"
#include "campaign.h"
#include "objecttype.h"
#include "pickuptype.h"
#include "unittype.h"
#include "song.h"
#include "sound.h"
#include "walltype.h"
#include "vehicletype.h"
#include "weapontype.h"
#include "../weapons/weapons.h"
#include "gametype.h"

using namespace std;


static cfg_opt_t mod_map_opts[] =
{
	CFG_STR((char*) "name", ((char*)""), CFGF_NONE),
	CFG_STR((char*) "title", ((char*)""), CFGF_NONE),
	CFG_INT((char*) "arcade", 1, CFGF_NONE),
	CFG_END()
};

static cfg_opt_t mod_opts[] =
{
	CFG_STR((char*) "title", ((char*)""), CFGF_NONE),
	CFG_SEC((char*) "map", mod_map_opts, CFGF_MULTI),
	CFG_INT((char*) "arcade", 1, CFGF_NONE),
	CFG_INT((char*) "campaign", 0, CFGF_NONE),
	CFG_END()
};



/**
* Inits the mod. Does not load yet.
* Directory should NOT contain a trailing slash
**/
Mod::Mod(GameState * st, string directory)
{
	this->st = st;
	
	int bp = directory.rfind('/');
	this->name = directory.substr(bp + 1, 100);
	
	directory.append("/");
	this->directory = directory;
	
	this->has_arcade = false;
	this->has_campaign = false;
	
	this->ais = NULL;
	this->gametypes = NULL;
	this->objecttypes = NULL;
	this->pickuptypes = NULL;
	this->songs = NULL;
	this->sounds = NULL;
	this->unitclasses = NULL;
	this->vehicletypes = NULL;
	this->walltypes = NULL;
	this->weapontypes = NULL;
	this->maps = NULL;
}

Mod::~Mod()
{
	delete(this->ais);
	delete(this->gametypes);
	delete(this->objecttypes);
	delete(this->pickuptypes);
	delete(this->songs);
	delete(this->sounds);
	delete(this->unitclasses);
	delete(this->walltypes);
	delete(this->vehicletypes);
	delete(this->weapontypes);
	delete(this->maps);
}


/**
* Load just metadata for this mod
**/
bool Mod::loadMetadata()
{
	cfg_t *cfg, *cfg_sub;
	int num_types, j;

	// Grab the map list from the mod
	char *buffer = this->loadText("mod.conf");
	if (buffer == NULL) {
		return false;
	}

	// Parse it
	cfg = cfg_init(mod_opts, CFGF_NONE);
	cfg_parse_buf(cfg, buffer);
	free(buffer);

	// Basic metadata
	this->title = cfg_getstr(cfg, "title");
	this->has_arcade = (cfg_getint(cfg, "arcade") == 1);
	this->has_campaign = (cfg_getint(cfg, "campaign") == 1);

	// Maps
	delete(this->maps);
	this->maps = new vector<MapReg>();
	
	num_types = cfg_size(cfg, "map");
	for (j = 0; j < num_types; j++) {
		cfg_sub = cfg_getnsec(cfg, "map", j);
		
		this->maps->push_back(MapReg(
			cfg_getstr(cfg_sub, "name"),
			cfg_getstr(cfg_sub, "title"),
			this,
			cfg_getint(cfg_sub, "arcade") == 1
		));
	}

	cfg_free(cfg);

	return true;
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
*   ui          UI to update while loading
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
vector<T> * loadModFile(Mod* mod, UIUpdate* ui, const char* filename, const char* section, cfg_opt_t* item_opts, T (*item_f)(cfg_t*, Mod*))
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
		return models;
	}
	
	cfg = cfg_init(opts, CFGF_NONE);
	cfg_parse_buf(cfg, buffer);
	free(buffer);
	
	int num_types = cfg_size(cfg, section);
	if (num_types == 0) {
		cerr << "Error loading file " << filename << ". File does not contain any '" << section << "' sections.\n";
		return models;
	}

	mod->setLoadErr("Unknown error");
	
	// Process area type sections
	int j;
	char buf[128];
	for (j = 0; j < num_types; j++) {
		cfg_item = cfg_getnsec(cfg, section, j);
		
		T am = (*item_f)(cfg_item, mod);
		if (am == NULL) {
			cerr << "Bad definition in file " << filename << " at index " << j << ": " << mod->getLoadErr() << "\n";
			return NULL;
		}
		
		strncpy(buf, mod->name.c_str(), 64);
		strncpy(buf + 64, am->name.c_str(), 64);
		am->id = crc32(0, buf, 128);
		
		models->push_back(am);
		
		DEBUG("mod", "%08x %s", am->id, am->name.c_str());
	}

	if (ui) ui->updateUI();

	cfg_free(cfg);
	return models;
}


/**
* To be called by the subitems of the mod to set an error message
**/
void Mod::setLoadErr(const char *message, ...)
{
	char buf[255];
	va_list argptr;
	
	va_start(argptr, message);
	vsnprintf(buf, 255, message, argptr);
	va_end(argptr);
	
	this->load_err = std::string(buf);
}


/**
* Loads the mod
**/
bool Mod::load(UIUpdate* ui)
{
	DEBUG("mod", "Loading mod '%s'", this->name.c_str());
	
#ifdef NOGUI
	sounds = new vector<Sound*>();
	songs = new vector<Song*>();
	
#else
	sounds = loadModFile<Sound*>(this, ui, "sounds.conf", "sound", sound_opts, &loadItemSound);
	if (sounds == NULL) return false;
	
	songs = loadModFile<Song*>(this, ui, "songs.conf", "song", song_opts, &loadItemSong);
	if (songs == NULL) return false;
#endif
	
	objecttypes = loadModFile<ObjectType*>(this, ui, "objecttypes.conf", "objecttype", objecttype_opts, &loadItemObjectType);
	if (objecttypes == NULL) return false;
	
	walltypes = loadModFile<WallType*>(this, ui, "walltypes.conf", "walltype", walltype_opts, &loadItemWallType);
	if (walltypes == NULL) return false;
	
	weapontypes = loadModFile<WeaponType*>(this, ui, "weapontypes.conf", "weapon", weapontype_opts, &loadItemWeaponType);
	if (weapontypes == NULL) return false;
	
	vehicletypes = loadModFile<VehicleType*>(this, ui, "vehicletypes.conf", "vehicletype", vehicletype_opts, &loadItemVehicleType);
	if (vehicletypes == NULL) return false;
	
	unitclasses = loadModFile<UnitType*>(this, ui, "unittypes.conf", "unittype", unittype_opts, &loadItemUnitType);
	if (unitclasses == NULL) return false;
	
	ais = loadModFile<AIType*>(this, ui, "ais.conf", "ai", ai_opts, &loadItemAIType);
	if (ais == NULL) return false;
	
	gametypes = loadModFile<GameType*>(this, ui, "gametypes.conf", "gametype", gametype_opts, &loadItemGameType);
	if (gametypes == NULL) return false;
	
	campaigns = loadModFile<Campaign*>(this, ui, "campaigns.conf", "campaign", campaign_opts, &loadItemCampaign);
	if (campaigns == NULL) return false;
	
	pickuptypes = loadModFile<PickupType*>(this, ui, "pickuptypes.conf", "pickuptype", pickuptype_opts, &loadItemPickupType);
	if (pickuptypes == NULL) return false;

	// Auto-create pickup types for weapons, ammocrates
	for (int i = weapontypes->size() - 1; i >= 0; --i) {
		string tmp;
		AssimpModel *model;
		
		tmp = "weapon_" + weapontypes->at(i)->name;
		
		model = this->getAssimpModel("pickup_" + tmp + ".dae");
		if (model == NULL) {
			model = this->getAssimpModel("pickup_weapon_generic.dae");
		}
		
		PickupType* pt = new PickupType();
		pt->name = tmp;
		pt->type = PICKUP_TYPE_WEAPON;
		pt->wt = weapontypes->at(i);
		pt->setModel(model);
		
		pickuptypes->push_back(pt);
		
		
		tmp = "ammo_" + weapontypes->at(i)->name;
		
		model = this->getAssimpModel("pickup_" + tmp + ".dae");
		if (model == NULL) {
			model = this->getAssimpModel("pickup_ammo_generic.dae");
		}
		
		pt = new PickupType();
		pt->name = tmp;
		pt->type = PICKUP_TYPE_AMMO;
		pt->wt = weapontypes->at(i);
		pt->setModel(model);
		
		pickuptypes->push_back(pt);
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
	vector<WeaponType*> * n_weapontypes = loadModFile<WeaponType*>(this, NULL, "weapontypes.conf", "weapon", weapontype_opts, &loadItemWeaponType);
	if (n_weapontypes == NULL) return false;
	
	for (i = n_weapontypes->size() - 1; i >= 0; --i) {
		WeaponType *nu = n_weapontypes->at(i);
		WeaponType *old = this->getWeaponType(nu->name);
		if (! old) continue;
		
		old->title = nu->title;
		old->fire_delay = nu->fire_delay;
		old->reload_delay = nu->reload_delay;
		old->continuous = nu->continuous;
		old->magazine_limit = nu->magazine_limit;
		old->belt_limit = nu->belt_limit;
	}
	
	return true;
}



/**
* Gets an AIType by ID
**/
AIType * Mod::getAIType(CRC32 id)
{
	for (int i = ais->size() - 1; i >= 0; --i) {
		if (ais->at(i)->id == id) return ais->at(i);
	}
	return NULL;
}

AIType * Mod::getAIType(string name)
{
	if (name.empty()) return NULL;
	
	for (int i = ais->size() - 1; i >= 0; --i) {
		if (ais->at(i)->name.compare(name) == 0) return ais->at(i);
	}
	
	return NULL;
}


/**
* Load an assimp model
**/
AssimpModel * Mod::getAssimpModel(string name)
{
	map<string, AssimpModel*>::iterator it = this->models.find(name);
	if (it != this->models.end()) {
		return it->second;
	}
	
	AssimpModel* am = new AssimpModel(this, name);
	
	if (! am->load((Render3D*) GEng()->render, false)) {
		delete(am);
		return NULL;
	}
	
	this->models.insert(std::pair<string, AssimpModel*>(name, am));

	return am;
}



/**
* Return a campaign by it's name
**/
Campaign * Mod::getCampaign(string name)
{
	if (name.empty()) return NULL;
	
	int i;
	for (i = campaigns->size() - 1; i >= 0; --i) {
		if (campaigns->at(i)->name.compare(name) == 0) return campaigns->at(i);
	}
	return NULL;
}


/**
* Get the list of campaigns
**/
vector<Campaign*> * Mod::getCampaigns()
{
	return this->campaigns;
}


/**
* Gets a gametype by name
**/
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
ObjectType * Mod::getObjectType(CRC32 id)
{
	for (int i = objecttypes->size() - 1; i >= 0; --i) {
		if (objecttypes->at(i)->id == id) return objecttypes->at(i);
	}
	return NULL;
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

void Mod::addObjectType(ObjectType * ot)
{
	objecttypes->push_back(ot);
}


/**
* Gets a vehicle type by ID
**/
PickupType * Mod::getPickupType(CRC32 id)
{
	for (int i = pickuptypes->size() - 1; i >= 0; --i) {
		if (pickuptypes->at(i)->id == id) return pickuptypes->at(i);
	}
	return NULL;
}

/**
* Gets a pickup type by name
**/
PickupType * Mod::getPickupType(string name)
{
	if (name.empty()) return NULL;
	
	int i;
	for (i = pickuptypes->size() - 1; i >= 0; --i) {
		if (pickuptypes->at(i)->name.compare(name) == 0) return pickuptypes->at(i);
	}
	return NULL;
}


/**
* Gets a unitclass by ID
**/
UnitType * Mod::getUnitType(CRC32 id)
{
	for (int i = unitclasses->size() - 1; i >= 0; --i) {
		if (unitclasses->at(i)->id == id) return unitclasses->at(i);
	}
	return NULL;
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
* Gets a vehicle type by ID
**/
VehicleType * Mod::getVehicleType(CRC32 id)
{
	for (int i = vehicletypes->size() - 1; i >= 0; --i) {
		if (vehicletypes->at(i)->id == id) return vehicletypes->at(i);
	}
	return NULL;
}

/**
* Gets a vehicle type by name
**/
VehicleType * Mod::getVehicleType(string name)
{
	if (name.empty()) return NULL;
	
	int i;
	for (i = vehicletypes->size() - 1; i >= 0; --i) {
		if (vehicletypes->at(i)->name.compare(name) == 0) return vehicletypes->at(i);
	}
	return NULL;
}


/**
* Gets a unitclass by name
**/
Song * Mod::getSong(string name)
{
	if (name.empty()) return NULL;
	
	int i;
	for (i = songs->size() - 1; i >= 0; --i) {
		if (songs->at(i)->name.compare(name) == 0) return songs->at(i);
	}
	return NULL;
}

/**
* Gets a random song
**/
Song * Mod::getRandomSong()
{
	if (songs == NULL or songs->size() == 0) return NULL;
	return songs->at(getRandom(0, songs->size() - 1));
}


/**
* Gets a sound by name
**/
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
WallType * Mod::getWallType(CRC32 id)
{
	for (int i = walltypes->size() - 1; i >= 0; --i) {
		if (walltypes->at(i)->id == id) return walltypes->at(i);
	}
	return NULL;
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
* Gets a weapon type by CRC32 ID
**/
WeaponType * Mod::getWeaponType(CRC32 id)
{
	for (int i = weapontypes->size() - 1; i >= 0; --i) {
		if (weapontypes->at(i)->id == id) return weapontypes->at(i);
	}
	return NULL;
}

/**
* Gets a weapon type by name
**/
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
* Returns two iterators for getting all weapon types
**/
void Mod::getAllWeaponTypes(vector<WeaponType*>::iterator * start, vector<WeaponType*>::iterator * end)
{
	*start = weapontypes->begin();
	*end = weapontypes->end();
}


/**
* Loads a text file into a char * pointer.
* Don't forget to free() when you are done.
**/
char * Mod::loadText(string resname)
{
	SDL_RWops *rw;
	char *buffer;
	
	rw = this->loadRWops(resname);
	if (rw == NULL) return NULL;
	
	Sint64 length = SDL_RWseek(rw, 0, SEEK_END);
	if (length == -1) {
		SDL_RWclose(rw);
		return NULL;
	}
	
	buffer = (char*) malloc(length + 1);
	if (buffer == NULL) {
		SDL_RWclose(rw);
		return NULL;
	}
	buffer[length] = '\0';
	
	SDL_RWseek(rw, 0, SEEK_SET);
	if (SDL_RWread(rw, buffer, length, 1) == 0) {
		SDL_RWclose(rw);
		return NULL;
	}
	
	SDL_RWclose(rw);
	
	return buffer;
}


/**
* Loads binary data from the mod.
**/
Uint8 * Mod::loadBinary(string resname, Sint64 *len)
{
	SDL_RWops *rw;
	Uint8 *buffer;
	
	rw = this->loadRWops(resname);
	if (rw == NULL) return NULL;
	
	Sint64 length = SDL_RWseek(rw, 0, SEEK_END);
	if (length == -1) {
		SDL_RWclose(rw);
		return NULL;
	}
	
	buffer = (Uint8*) malloc(length);
	if (buffer == NULL) {
		SDL_RWclose(rw);
		return NULL;
	}
	
	SDL_RWseek(rw, 0, SEEK_SET);
	if (SDL_RWread(rw, buffer, length, 1) == 0) {
		SDL_RWclose(rw);
		return NULL;
	}
	
	SDL_RWclose(rw);
	
	*len = length;
	return buffer;
}


/**
* Loads a resource into a SDL_RWops.
* Don't forget to SDL_RWclose() when you are done.
**/
SDL_RWops * Mod::loadRWops(string resname)
{
	string filename = directory;
	filename.append(resname);
	
	return SDL_RWFromFile(filename.c_str(), "rb");
}




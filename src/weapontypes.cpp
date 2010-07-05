// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include <confuse.h>
#include <zzip/zzip.h>
#include "rage.h"

using namespace std;


// It is expected you will change the HUD code if you change these values
#define WEAPON_LARGE_W 125
#define WEAPON_LARGE_H 125



/* Variables */
static vector<WeaponType*> weapons;

/* Functions */
WeaponType* loadWeaponType(cfg_t *cfg_weapon, Render * render);


/* Config file definition */
// Weapon section
static cfg_opt_t weapon_opts[] =
{
	CFG_STR((char*) "name", 0, CFGF_NONE),
	CFG_INT((char*) "particlegen", -1, CFGF_NONE),
	CFG_END()
};

// Main config
static cfg_opt_t opts[] =
{
	CFG_SEC((char*) "weapon", weapon_opts, CFGF_MULTI),
	CFG_END()
};




WeaponType::WeaponType()
{
	this->pg = NULL;
	this->icon_large = NULL;
}


/**
* Loads the area types
**/
bool loadAllWeaponTypes(Render * render)
{
	char *buffer;
	ZZIP_FILE *fp;
	cfg_t *cfg, *cfg_weapon;
	
	
	// Load the 'info.conf' file
	string filename = getDataDirectory(DF_WEAPONS);
	filename.append("weapontypes.conf");
	fp = zzip_open(filename.c_str(), 0);
	if (! fp) {
		cerr << "Can't read weapontypes.conf file\n";
		return false;
	}
	
	// Read the contents of the file into a buffer
	zzip_seek (fp, 0, SEEK_END);
	int len = zzip_tell (fp);
	zzip_seek (fp, 0, SEEK_SET);
	
	buffer = (char*) malloc(len + 1);
	if (buffer == NULL) {
		cerr << "Can't read weapontypes.conf file\n";
		return false;
	}
	buffer[len] = '\0';
	
	zzip_read(fp, buffer, len);
	zzip_close(fp);
	
	
	// Parse config file
	cfg = cfg_init(opts, CFGF_NONE);
	cfg_parse_buf(cfg, buffer);
	
	int num_types = cfg_size(cfg, "weapon");
	if (num_types == 0) return false;
	
	// Process area type sections
	int j;
	for (j = 0; j < num_types; j++) {
		cfg_weapon = cfg_getnsec(cfg, "weapon", j);
		
		WeaponType* wt = loadWeaponType(cfg_weapon, render);
		if (wt == NULL) {
			cerr << "Bad weapon type at index " << j << endl;
			return false;
		}
		
		weapons.push_back(wt);
		wt->id = weapons.size() - 1;
	}
	
	// If there was sprite errors, exit the game
	if (render->wasLoadSpriteError()) {
		cerr << "Error loading weapon types; game will now exit.\n";
		exit(1);
	}
	
	return true;
}


/**
* Loads a single area type
**/
WeaponType* loadWeaponType(cfg_t *cfg_weapon, Render * render)
{
	WeaponType* wt;
	string filename;
	
	wt = new WeaponType();
	
	if (cfg_getint(cfg_weapon, "particlegen") != -1) {
		wt->pg = getParticleGenTypeByID(cfg_getint(cfg_weapon, "particlegen"));
	}
	
	// Load large icon
	filename = getDataDirectory(DF_WEAPONS);
	filename.append(cfg_getstr(cfg_weapon, "name"));
	filename.append("/icon_large.bmp");
	wt->icon_large = render->loadSprite(filename);
	
	if (render->getSpriteWidth(wt->icon_large) != WEAPON_LARGE_W && render->getSpriteHeight(wt->icon_large) != WEAPON_LARGE_H) {
		cout << "Bad image size for large icon\n";
		return NULL;
	}
	
	return wt;
}


WeaponType* getWeaponTypeByID(unsigned int id)
{
	if (id < 0 or id > weapons.size()) return NULL;
	return weapons.at(id);
}

unsigned int getNumWeaponTypes()
{
	return weapons.size();
}



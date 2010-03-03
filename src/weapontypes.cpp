#include <iostream>
#include <SDL.h>
#include <confuse.h>
#include <zzip/zzip.h>
#include "rage.h"

using namespace std;


/* Variables */
static vector<WeaponType*> weapons;

/* Functions */
WeaponType* loadWeaponType(cfg_t *cfg_weapon);


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
}


/**
* Loads the area types
**/
bool loadAllWeaponTypes()
{
	char *buffer;
	ZZIP_FILE *fp;
	cfg_t *cfg, *cfg_weapon;
	
	
	// Load the 'info.conf' file
	fp = zzip_open("weapontypes/weapontypes.conf", 0);
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
		
		WeaponType* wt = loadWeaponType(cfg_weapon);
		if (wt == NULL) {
			cerr << "Bad weapon type at index " << j << endl;
			return false;
		}
		
		weapons.push_back(wt);
		wt->id = weapons.size() - 1;
	}
	
	// If there was sprite errors, exit the game
	if (wasLoadSpriteError()) {
		cerr << "Error loading weapon types; game will now exit.\n";
		exit(1);
	}
	
	return true;
}


/**
* Loads a single area type
**/
WeaponType* loadWeaponType(cfg_t *cfg_weapon)
{
	WeaponType* wt;
	
	if (cfg_getint(cfg_weapon, "particlegen") == -1) return NULL;
	
	wt = new WeaponType();
	wt->pg = getParticleGenTypeByID(cfg_getint(cfg_weapon, "particlegen"));
	
	return wt;
}


WeaponType* getWeaponTypeByID(unsigned int id)
{
	if (id < 0 or id > weapons.size()) return NULL;
	return weapons.at(id);
}



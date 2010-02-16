#include <iostream>
#include <SDL.h>
#include <confuse.h>
#include <zzip/zzip.h>
#include "rage.h"

using namespace std;


/* Variables */
static vector<UnitClass*> unitclasses;


/* Config file definition */
// Settings section
static cfg_opt_t settings_opts[] =
{
	CFG_INT((char*) "lin_speed", 0, CFGF_NONE),
	CFG_INT((char*) "lin_accel", 0, CFGF_NONE),
	CFG_INT((char*) "turn_speed", 0, CFGF_NONE),
	CFG_END()
};

// Unitclass section
static cfg_opt_t unitclass_opts[] =
{
	CFG_STR((char*) "name", 0, CFGF_NONE),
	CFG_SEC((char*) "settings", settings_opts, CFGF_MULTI),
	CFG_END()
};

// Main config
static cfg_opt_t opts[] =
{
	CFG_SEC((char*) "unitclass", unitclass_opts, CFGF_MULTI),
	CFG_END()
};


/**
* Loads the area types
**/
bool loadAllUnitClasses()
{
	char *buffer;
	ZZIP_FILE *fp;
	cfg_t *cfg, *cfg_unitclass;
	
	
	// Load the 'info.conf' file
	fp = zzip_open("unitclass/unitclass.conf", 0);
	if (! fp) {
		cerr << "Can't read unitclass.conf file\n";
		return false;
	}
	
	// Read the contents of the file into a buffer
	zzip_seek (fp, 0, SEEK_END);
	int len = zzip_tell (fp);
	zzip_seek (fp, 0, SEEK_SET);
	
	buffer = (char*) malloc(len + 1);
	if (buffer == NULL) {
		cerr << "Can't read unitclass.conf file\n";
		return false;
	}
	buffer[len] = '\0';
	
	zzip_read(fp, buffer, len);
	zzip_close(fp);
	
	
	// Parse config file
	cfg = cfg_init(opts, CFGF_NONE);
	cfg_parse_buf(cfg, buffer);
	
	int num_types = cfg_size(cfg, "unitclass");
	if (num_types == 0) return false;
	
	// Process area type sections
	int j;
	for (j = 0; j < num_types; j++) {
		cfg_unitclass = cfg_getnsec(cfg, "unitclass", j);
		
		UnitClass* uc = loadUnitClass(cfg_unitclass);
		if (uc == NULL) {
			cerr << "Bad unit class at index " << j << endl;
			return false;
		}
		
		unitclasses.push_back(uc);
		uc->id = unitclasses.size() - 1;
	}
	
	// If there was sprite errors, exit the game
	if (wasLoadSpriteError()) {
		cerr << "Error loading unit classes; game will now exit.\n";
		exit(1);
	}
	
	
	return true;
}



/**
* Loads a single area type
**/
UnitClass* loadUnitClass(cfg_t *cfg)
{
	UnitClass* uc;
	cfg_t *cfg_settings;
	
	
	uc = new UnitClass();
	uc->name = cfg_getstr(cfg, "name");
	
	
	int num_settings = cfg_size(cfg, "settings");
	if (num_settings - 1 != UNIT_NUM_MODIFIERS) return NULL;
	
	// Load initial config
	cfg_settings = cfg_getnsec(cfg, "settings", 0);
	uc->initial.lin_speed = cfg_getint(cfg_settings, "lin_speed");
	uc->initial.lin_accel = cfg_getint(cfg_settings, "lin_accel");
	uc->initial.turn_speed = cfg_getint(cfg_settings, "turn_speed");
	
	if (uc->initial.lin_speed == 0) return NULL;
	if (uc->initial.lin_accel == 0) return NULL;
	if (uc->initial.turn_speed == 0) return NULL;
	
	// load modifiers
	int j;
	for (j = 1; j < num_settings; j++) {
		cfg_settings = cfg_getnsec(cfg, "settings", j);
		
		uc->mod[j].lin_speed = cfg_getint(cfg_settings, "lin_speed");
		uc->mod[j].lin_accel = cfg_getint(cfg_settings, "lin_accel");
		uc->mod[j].turn_speed = cfg_getint(cfg_settings, "turn_speed");
	}
	
	return uc;
}


UnitClass* getUnitClassByID(int id)
{
	return unitclasses.at(id);
}



/**
* Returns the settings to use for any given set of modifier flags.
* The returned object should be freed by the caller.
**/
UnitClassSettings* UnitClass::getSettings(Uint8 modifier_flags)
{
	UnitClassSettings *ret;
	
	ret = new UnitClassSettings();
	
	ret->lin_speed = this->initial.lin_speed;
	ret->lin_accel = this->initial.lin_accel;
	ret->turn_speed = this->initial.turn_speed;
	
	for (int i = 0; i < UNIT_NUM_MODIFIERS; i++) {
		if ((modifier_flags & (1 << i)) != 0) {
			ret->lin_speed += this->mod[i].lin_speed;
			ret->lin_accel += this->mod[i].lin_accel;
			ret->turn_speed += this->mod[i].turn_speed;
		}
	}
	
	return ret;
}



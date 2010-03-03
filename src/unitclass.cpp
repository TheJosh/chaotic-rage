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

// State section
static cfg_opt_t state_opts[] =
{
	CFG_STR((char*) "image", 0, CFGF_NONE),
	CFG_INT((char*) "type", 0, CFGF_NONE),
	CFG_INT((char*) "num_frames", 0, CFGF_NONE),
	CFG_END()
};

// Unitclass section
static cfg_opt_t unitclass_opts[] =
{
	CFG_STR((char*) "name", 0, CFGF_NONE),
	CFG_SEC((char*) "settings", settings_opts, CFGF_MULTI),
	CFG_SEC((char*) "state", state_opts, CFGF_MULTI),
	CFG_END()
};

// Main config
static cfg_opt_t opts[] =
{
	CFG_SEC((char*) "unitclass", unitclass_opts, CFGF_MULTI),
	CFG_END()
};



UnitClass::UnitClass()
{
	this->width = 0;
	this->height = 0;
}

UnitClass::~UnitClass()
{
}


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
	cfg_t *cfg_settings, *cfg_state;
	int j;
	
	
	uc = new UnitClass();
	uc->name = cfg_getstr(cfg, "name");
	
	
	/// Settings ///
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
	for (j = 1; j < num_settings; j++) {
		cfg_settings = cfg_getnsec(cfg, "settings", j);
		
		uc->mod[j - 1].lin_speed = cfg_getint(cfg_settings, "lin_speed");
		uc->mod[j - 1].lin_accel = cfg_getint(cfg_settings, "lin_accel");
		uc->mod[j - 1].turn_speed = cfg_getint(cfg_settings, "turn_speed");
	}
	
	
	
	/// States ///
	int num_states = cfg_size(cfg, "state");
	if (num_states < 1) return NULL;
	
	// load states
	uc->max_frames = 0;
	for (j = 0; j < num_states; j++) {
		cfg_state = cfg_getnsec(cfg, "state", j);
		
		UnitClassState* uct = new UnitClassState();
		
		uct->image = cfg_getstr(cfg_state, "image");
		uct->type = cfg_getint(cfg_state, "type");
		uct->num_frames = cfg_getint(cfg_state, "num_frames");
		
		uc->states.push_back(uct);
		uct->id = uc->states.size() - 1;
		
		uc->max_frames = MAX(uc->max_frames, uct->num_frames);
	}
	
	// Assign sprite offsets
	for (j = 0; j < num_states; j++) {
		uc->states.at(j)->sprite_offset = j * uc->max_frames * 8;
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
			ret->lin_speed += this->mod[i - 1].lin_speed;
			ret->lin_accel += this->mod[i - 1].lin_accel;
			ret->turn_speed += this->mod[i - 1].turn_speed;
		}
	}
	
	return ret;
}


/**
* Returns a random state which matches the specified type.
* If no state for the specified type is found, uses a state from the UNIT_STATE_STATIC type.
**/
UnitClassState* UnitClass::getState(int type)
{
	unsigned int j = 0;
	unsigned int num = 0;
	
	// Find out how many of this time exist
	for (j = 0; j < this->states.size(); j++) {
		if (this->states.at(j)->type == type) num++;
	}
	
	// Randomly choose one
	num = getRandom(0, num);
	for (j = 0; j < this->states.size(); j++) {
		if (this->states.at(j)->type == type) {
			if (num == 0) {
				return this->states.at(j);
			}
			num--;
		}
	}
	
	// If no state of this type found, do a search for a static type
	if (type == UNIT_STATE_STATIC) {
		cerr << "Cannot find state of type UNIT_STATE_STATIC for unit.\n";
		exit(1);
	}
	
	return this->getState(UNIT_STATE_STATIC);
}


unsigned int UnitClass::getMaxFrames()
{
	return this->max_frames;
}

/**
* Loads all of the required state sprites into SDL_Surface's.
* The returned object should be freed by the caller.
**/
vector<SDL_Surface*>* UnitClass::loadAllSprites()
{
	vector<SDL_Surface*>* ret = new vector<SDL_Surface*>();
	
	unsigned int state;
	unsigned int angle;
	unsigned int frame;
	
	UnitClassState* state_info;
	char buff[255];
	
	for (state = 0; state < this->states.size(); state++) {
		state_info = this->states.at(state);
		
		for (angle = 0; angle < 8; angle++) {
			for (frame = 0; frame < state_info->num_frames; frame++) {
				
				sprintf(buff, "unitclass/%s/%s_%ideg_fr%i.bmp", this->name.c_str(), state_info->image.c_str(), angle * 45, frame);
				
				DEBUG("Loading unit class sprite; image = '%s', angle = %i, frame = %i\n", state_info->image.c_str(), angle * 45, frame);
				
				SDL_Surface *surf = loadSprite(buff);
				ret->push_back(surf);
				
				if (this->width == 0) {
					this->width = surf->w;
					this->height = surf->h;
				}
			}
			
			while (frame < this->max_frames) {
				ret->push_back(NULL);
				frame++;
			}
		}
	}
	
	return ret;
}



#include <iostream>
#include <SDL.h>
#include <confuse.h>
#include <zzip/zzip.h>
#include "rage.h"

using namespace std;


/* Variables */
static vector<ParticleType*> particletypes;

/* Functions */
ParticleType* loadParticleType(cfg_t *cfg_particletype);


/* Config file definition */
// Areatype section
static cfg_opt_t particletype_opts[] =
{
	CFG_STR((char*) "name", 0, CFGF_NONE),
	CFG_INT((char*) "directional", 0, CFGF_NONE),		// 1 = use direction info, 0 = only use angle 0deg
	CFG_INT((char*) "num_frames", 1, CFGF_NONE),
	CFG_INT((char*) "lin_speed", 0, CFGF_NONE),
	CFG_INT((char*) "lin_accel", 0, CFGF_NONE),
	CFG_END()
};

// Main config
static cfg_opt_t opts[] =
{
	CFG_SEC((char*) "particle", particletype_opts, CFGF_MULTI),
	CFG_END()
};




ParticleType::ParticleType()
{
}


/**
* Loads the area types
**/
bool loadAllParticleTypes()
{
	char *buffer;
	ZZIP_FILE *fp;
	cfg_t *cfg, *cfg_particletype;
	
	
	// Load the 'info.conf' file
	fp = zzip_open("particletypes/particletypes.conf", 0);
	if (! fp) {
		cerr << "Can't read particletypes.conf file\n";
		return false;
	}
	
	// Read the contents of the file into a buffer
	zzip_seek (fp, 0, SEEK_END);
	int len = zzip_tell (fp);
	zzip_seek (fp, 0, SEEK_SET);
	
	buffer = (char*) malloc(len + 1);
	if (buffer == NULL) {
		cerr << "Can't read particletypes.conf file\n";
		return false;
	}
	buffer[len] = '\0';
	
	zzip_read(fp, buffer, len);
	zzip_close(fp);
	
	
	// Parse config file
	cfg = cfg_init(opts, CFGF_NONE);
	cfg_parse_buf(cfg, buffer);
	
	int num_types = cfg_size(cfg, "particle");
	if (num_types == 0) return false;
	
	// Process area type sections
	int j;
	for (j = 0; j < num_types; j++) {
		cfg_particletype = cfg_getnsec(cfg, "particle", j);
		
		ParticleType* pt = loadParticleType(cfg_particletype);
		if (pt == NULL) {
			cerr << "Bad particle type at index " << j << endl;
			return false;
		}
		
		particletypes.push_back(pt);
		pt->id = particletypes.size() - 1;
	}
	
	// If there was sprite errors, exit the game
	if (wasLoadSpriteError()) {
		cerr << "Error loading particle types; game will now exit.\n";
		exit(1);
	}
	
	return true;
}


/**
* Loads a single area type
**/
ParticleType* loadParticleType(cfg_t *cfg_particletype)
{
	ParticleType* pt;
	string filename;
	char buff[255];
	
	if (cfg_size(cfg_particletype, "name") == 0) return NULL;
	if (cfg_getint(cfg_particletype, "num_frames") < 1) return NULL;
	if (cfg_getint(cfg_particletype, "lin_speed") == 0) return NULL;
	if (cfg_getint(cfg_particletype, "lin_accel") == 0) return NULL;
	
	// Load settings
	pt = new ParticleType();
	pt->name = cfg_getstr(cfg_particletype, "name");
	pt->directional = cfg_getint(cfg_particletype, "directional");
	pt->num_frames = cfg_getint(cfg_particletype, "num_frames");
	
	// Load sprites
	int angle;
	int frame;
	for (angle = 0; angle < 8; angle++) {
		for (frame = 0; frame < pt->num_frames; frame++) {
			
			sprintf(buff, "particletypes/%s/%ideg_fr%i.bmp", pt->name.c_str(), angle * 45, frame);
			
			DEBUG("Loading particle type sprite; name = '%s', angle = %i, frame = %i\n", pt->name.c_str(), angle * 45, frame);
			
			SDL_Surface *surf = loadSprite(buff);
			pt->sprites.push_back(surf);
			
		}
		
		if (pt->directional == 0) angle = 8;
	}
	
	return pt;
}


ParticleType* getParticleTypeByID(int id)
{
	return particletypes.at(id);
}



#include <iostream>
#include <SDL.h>
#include <confuse.h>
#include <zzip/zzip.h>
#include "rage.h"

using namespace std;


/* Variables */
static vector<ParticleType*> particletypes;
static vector<ParticleGenType*> generatortypes;

/* Functions */
ParticleType* loadParticleType(cfg_t *cfg_particletype, Render * render);
ParticleGenType* loadParticleGenType(cfg_t *cfg_generatortype);

extern cfg_opt_t g_action_opts;

/* Config file definition */
// Particle section
static cfg_opt_t particletype_opts[] =
{
	CFG_STR((char*) "name", 0, CFGF_NONE),
	CFG_STR((char*) "image", 0, CFGF_NONE),
	CFG_INT((char*) "directional", 0, CFGF_NONE),		// 1 = use direction info, 0 = only use angle 0deg
	CFG_INT((char*) "num_frames", 1, CFGF_NONE),
	CFG_INT_LIST((char*) "lin_speed", 0, CFGF_NONE),
	CFG_INT_LIST((char*) "lin_accel", 0, CFGF_NONE),
	CFG_INT_LIST((char*) "age", 0, CFGF_NONE),
	CFG_INT_LIST((char*) "unit_damage", 0, CFGF_NONE),
	CFG_INT_LIST((char*) "wall_damage", 0, CFGF_NONE),
	
	CFG_SEC((char*) "action", &g_action_opts, CFGF_MULTI),
	
	CFG_END()
};

// Particle generator spewage
static cfg_opt_t spew_opts[] =
{
	CFG_STR((char*) "name", 0, CFGF_NONE),
	CFG_INT((char*) "type", 0, CFGF_NONE),
	CFG_INT((char*) "angle_range", 0, CFGF_NONE),
	CFG_INT((char*) "rate", 0, CFGF_NONE),
	CFG_INT((char*) "time", 0, CFGF_NONE),
	CFG_INT((char*) "offset", 0, CFGF_NONE),
	CFG_END()
};

// Particle generator section
static cfg_opt_t generatortype_opts[] =
{
	CFG_STR((char*) "name", 0, CFGF_NONE),
	CFG_SEC((char*) "spew", spew_opts, CFGF_MULTI),
	CFG_END()
};

// Main config - particletypes.conf
static cfg_opt_t opts_particles[] =
{
	CFG_SEC((char*) "particle", particletype_opts, CFGF_MULTI),
	CFG_END()
};

// Main config - particlegenerators.conf
static cfg_opt_t opts_generators[] =
{
	CFG_SEC((char*) "generator", generatortype_opts, CFGF_MULTI),
	CFG_END()
};



ParticleType::ParticleType() : EntityType()
{
}

ParticleType::~ParticleType()
{
}


/**
* Loads the area types
**/
bool loadAllParticleTypes(Render * render)
{
	char *buffer;
	int len;
	string filename;
	ZZIP_FILE *fp;
	cfg_t *cfg, *cfg_particletype, *cfg_generatortype;
	
	
	// Load the 'particletypes.conf' file
	filename = getDataDirectory(DF_PARTICLES);
	filename.append("particletypes.conf");
	fp = zzip_open(filename.c_str(), 0);
	if (! fp) {
		cerr << "Can't read particletypes.conf file\n";
		return false;
	}
	
	// Read the contents of the file into a buffer
	zzip_seek (fp, 0, SEEK_END);
	len = zzip_tell (fp);
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
	cfg = cfg_init(opts_particles, CFGF_NONE);
	cfg_parse_buf(cfg, buffer);
	
	
	
	int num_types = cfg_size(cfg, "particle");
	if (num_types == 0) return false;
	
	// Process particle type sections
	int j;
	for (j = 0; j < num_types; j++) {
		cfg_particletype = cfg_getnsec(cfg, "particle", j);
		
		ParticleType* pt = loadParticleType(cfg_particletype, render);
		if (pt == NULL) {
			cerr << "Bad particle type at index " << j << endl;
			return false;
		}
		
		particletypes.push_back(pt);
		pt->id = particletypes.size() - 1;
	}
	
	
	
	// Load the 'particletypes.conf' file
	filename = getDataDirectory(DF_PARTICLES);
	filename.append("particlegenerators.conf");
	fp = zzip_open(filename.c_str(), 0);
	if (! fp) {
		cerr << "Can't read particletypes.conf file\n";
		return false;
	}
	
	// Read the contents of the file into a buffer
	zzip_seek (fp, 0, SEEK_END);
	len = zzip_tell (fp);
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
	cfg = cfg_init(opts_generators, CFGF_NONE);
	cfg_parse_buf(cfg, buffer);
	
	// Process generator type sections
	num_types = cfg_size(cfg, "generator");
	if (num_types == 0) return false;
	
	for (j = 0; j < num_types; j++) {
		cfg_generatortype = cfg_getnsec(cfg, "generator", j);
		
		ParticleGenType* gt = loadParticleGenType(cfg_generatortype);
		if (gt == NULL) {
			cerr << "Bad particle generator type at index " << j << endl;
			return false;
		}
		
		generatortypes.push_back(gt);
		gt->id = particletypes.size() - 1;
	}
	
	
	
	// If there was sprite errors, exit the game
	if (render->wasLoadSpriteError()) {
		cerr << "Error loading particle types; game will now exit.\n";
		exit(1);
	}
	
	return true;
}


/**
* Loads a single particle type
**/
ParticleType* loadParticleType(cfg_t *cfg_particletype, Render * render)
{
	ParticleType* pt;
	char buff[255];
	
	if (cfg_size(cfg_particletype, "image") == 0) return NULL;
	if (cfg_getint(cfg_particletype, "num_frames") < 1) return NULL;
	if (cfg_getint(cfg_particletype, "lin_speed") == 0) return NULL;
	if (cfg_getint(cfg_particletype, "lin_accel") == 0) return NULL;
	if (cfg_getint(cfg_particletype, "age") == 0) return NULL;
	
	// Load settings
	pt = new ParticleType();
	pt->image = cfg_getstr(cfg_particletype, "image");
	pt->directional = cfg_getint(cfg_particletype, "directional");
	pt->num_frames = cfg_getint(cfg_particletype, "num_frames");
	pt->lin_speed = cfg_getrange(cfg_particletype, "lin_speed");
	pt->lin_accel = cfg_getrange(cfg_particletype, "lin_accel");
	pt->age = cfg_getrange(cfg_particletype, "age");
	pt->unit_damage = cfg_getrange(cfg_particletype, "unit_damage");
	pt->wall_damage = cfg_getrange(cfg_particletype, "wall_damage");
	
	pt->actions = loadActions(cfg_particletype);
	
	// Load sprites
	int angle;
	int frame;
	for (angle = 0; angle < 8; angle++) {
		for (frame = 0; frame < pt->num_frames; frame++) {
			
			sprintf(buff, "%s%s/%ideg_fr%i.bmp", getDataDirectory(DF_PARTICLES).c_str(), pt->image.c_str(), angle * 45, frame);
			
			DEBUG("Loading particle type sprite; image = '%s', angle = %i, frame = %i\n", pt->image.c_str(), angle * 45, frame);
			
			SpritePtr surf = render->loadSprite(buff);
			pt->sprites.push_back(surf);
			
		}
		
		if (pt->directional == 0) angle = 8;
	}
	
	return pt;
}


/**
* Loads a single particle generator type
**/
ParticleGenType* loadParticleGenType(cfg_t *cfg_generatortype)
{
	ParticleGenType* gt;
	cfg_t *cfg_spew;
	int j;
	
	if (cfg_size(cfg_generatortype, "name") == 0) return NULL;
	if (cfg_size(cfg_generatortype, "spew") == 0) return NULL;
	
	gt = new ParticleGenType();
	
	int num_types = cfg_size(cfg_generatortype, "spew");
	for (j = 0; j < num_types; j++) {
		cfg_spew = cfg_getnsec(cfg_generatortype, "spew", j);
		
		if (cfg_getint(cfg_spew, "angle_range") == 0) return NULL;
		if (cfg_getint(cfg_spew, "rate") == 0) return NULL;
		
		GenSpew* spew = new GenSpew();
		spew->pt = getParticleTypeByID(cfg_getint(cfg_spew, "type"));
		spew->angle_range = cfg_getint(cfg_spew, "angle_range");
		spew->rate = cfg_getint(cfg_spew, "rate");
		spew->time = cfg_getint(cfg_spew, "time");
		spew->offset = cfg_getint(cfg_spew, "offset");
		
		gt->spewers.push_back(spew);
	}
	
	return gt;
}


ParticleType* getParticleTypeByID(int id)
{
	return particletypes.at(id);
}

ParticleGenType* getParticleGenTypeByID(int id)
{
	return generatortypes.at(id);
}



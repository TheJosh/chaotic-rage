#include <iostream>
#include <SDL.h>
#include <confuse.h>
#include <zzip/zzip.h>
#include "rage.h"

using namespace std;


/* Variables */
static vector<AreaType*> areatypes;

/* Functions */
AreaType* loadAreaType(cfg_t *cfg_areatype, Render * render);


/* Config file definition */
// Areatype section
static cfg_opt_t areatype_opts[] =
{
	CFG_STR((char*) "name", 0, CFGF_NONE),
	CFG_STR((char*) "image", 0, CFGF_NONE),
	CFG_INT((char*) "stretch", 0, CFGF_NONE),		// 0 = tile, 1 = stretch
	CFG_INT((char*) "wall", 0, CFGF_NONE),			// 0 = ground, 1 = wall
	CFG_INT((char*) "ground_type", -1, CFGF_NONE),	// Ground to place underneath this wall
	CFG_END()
};

// Main config
static cfg_opt_t opts[] =
{
	CFG_SEC((char*) "areatype", areatype_opts, CFGF_MULTI),
	CFG_END()
};




AreaType::AreaType()
{
	this->surf = NULL;
	this->ground_type = NULL;
}


/**
* Loads the area types
**/
bool loadAllAreaTypes(Render * render)
{
	char *buffer;
	ZZIP_FILE *fp;
	cfg_t *cfg, *cfg_areatype;
	
	
	// Load the 'info.conf' file
	string filename = getDataDirectory(DF_AREATYPES);
	filename.append("areatypes.conf");
	fp = zzip_open(filename.c_str(), 0);
	if (! fp) {
		cerr << "Can't read areatypes.conf file\n";
		return false;
	}
	
	// Read the contents of the file into a buffer
	zzip_seek (fp, 0, SEEK_END);
	int len = zzip_tell (fp);
	zzip_seek (fp, 0, SEEK_SET);
	
	buffer = (char*) malloc(len + 1);
	if (buffer == NULL) {
		cerr << "Can't read areatypes.conf file\n";
		return false;
	}
	buffer[len] = '\0';
	
	zzip_read(fp, buffer, len);
	zzip_close(fp);
	
	
	// Parse config file
	cfg = cfg_init(opts, CFGF_NONE);
	cfg_parse_buf(cfg, buffer);
	
	int num_types = cfg_size(cfg, "areatype");
	if (num_types == 0) return false;
	
	// Process area type sections
	int j;
	for (j = 0; j < num_types; j++) {
		cfg_areatype = cfg_getnsec(cfg, "areatype", j);
		
		AreaType* at = loadAreaType(cfg_areatype, render);
		if (at == NULL) {
			cerr << "Bad area type at index " << j << endl;
			return false;
		}
		
		areatypes.push_back(at);
		at->id = areatypes.size() - 1;
	}
	
	// If there was sprite errors, exit the game
	if (render->wasLoadSpriteError()) {
		cerr << "Error loading area types; game will now exit.\n";
		exit(1);
	}
	
	return true;
}


/**
* Loads a single area type
**/
AreaType* loadAreaType(cfg_t *cfg_areatype, Render * render)
{
	AreaType* at;
	string filename;
	
	filename = getDataDirectory(DF_AREATYPES);
	filename.append(cfg_getstr(cfg_areatype, "image"));
	filename.append("-fr0.bmp");
	
	at = new AreaType();
	at->surf = render->loadSprite(filename.c_str());
	at->stretch = cfg_getint(cfg_areatype, "stretch");
	at->wall = cfg_getint(cfg_areatype, "wall");
	
	AreaType *ground = getAreaTypeByID(cfg_getint(cfg_areatype, "ground_type"));
	if (ground != NULL && ground->wall == 0) {
		at->ground_type = ground;
	}
	
	return at;
}


/**
* Gets an areatype
* Returns NULL on error
**/
AreaType* getAreaTypeByID(int id)
{
	if (id < 0 or ((unsigned int) id) > areatypes.size()) return NULL;
	return areatypes.at(id);
}



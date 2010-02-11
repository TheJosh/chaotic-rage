#include <iostream>
#include <SDL.h>
#include <confuse.h>
#include <zzip/zzip.h>
#include "rage.h"

using namespace std;


/* Variables */
static vector<AreaType*> areatypes;

/* Functions */
AreaType* loadAreaType(cfg_t *cfg_areatype);


/* Config file definition */
// Areatype section
static cfg_opt_t areatype_opts[] =
{
	CFG_STR((char*) "tex", 0, CFGF_NONE),
	CFG_INT((char*) "stretch", 0, CFGF_NONE),		// 0 = tile, 1 = stretch
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
}


/**
* Loads the area types
**/
bool loadAllAreaTypes()
{
	char *buffer;
	ZZIP_FILE *fp;
	cfg_t *cfg, *cfg_areatype;
	
	
	// Load the 'info.conf' file
	fp = zzip_open("areatypes/areatypes.conf", 0);
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
		
		AreaType* at = loadAreaType(cfg_areatype);
		if (at != NULL) {
			areatypes.push_back(at);
		}
	}
	
	// If there was sprite errors, exit the game
	if (wasLoadSpriteError()) {
		cerr << "Error loading area types; game will now exit.\n";
		exit(1);
	}
	
	return true;
}


/**
* Loads a single area type
**/
AreaType* loadAreaType(cfg_t *cfg_areatype)
{
	AreaType* at;
	string filename;
	
	filename = "areatypes/";
	filename.append(cfg_getstr(cfg_areatype, "tex"));
	filename.append("-fr0.bmp");
	
	at = new AreaType();
	at->surf = loadSprite(filename.c_str());
	at->stretch = cfg_getint(cfg_areatype, "stretch");
	
	return at;
}


AreaType* getAreaTypeByID(int id)
{
	return areatypes.at(id);
}



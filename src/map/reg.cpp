// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <map>
#include <confuse.h>
#include "reg.h"
#include "../mod/mod.h"

using namespace std;


// Main config
static cfg_opt_t opts[] =
{
	CFG_STR((char*) "title", 0, CFGF_NONE),
	CFG_STR((char*) "preview", 0, CFGF_NONE),
	CFG_END()
};


/**
* Find maps in a given directory and add them to the map registry
**/
void MapRegistry::find(string dir)
{
	std::vector<string>* sysmaps;
	std::vector<string>::iterator it;

	sysmaps = getSystemMapNames();

	for (it = sysmaps->begin(); it != sysmaps->end(); ++it) {
		string filename = "maps/";
		filename.append(*it);
		filename.append("/metadata.conf");

		cfg_t* cfg = cfg_init(opts, CFGF_NONE);
		int result = cfg_parse(cfg, filename.c_str());

		if (result == CFG_PARSE_ERROR) {
			reportFatalError("Unable to parse map metadata " + filename);
		} else if (result == CFG_SUCCESS) {
			string title = cfg_getstr(cfg, "title");
			maps.push_back(MapReg(*it, title));

			char* preview = cfg_getstr(cfg, "preview");
			if (preview != NULL) {
				filename = "maps/";
				filename.append(*it);
				filename.append("/");
				filename.append(preview);
				maps.back().preview = filename;
			}
		}

		cfg_free(cfg);
	}

	delete(sysmaps);
}


/**
* Find maps which are contained within a specified mod
**/
void MapRegistry::find(Mod* mod)
{
	mod->loadMetadata();
	vector<MapReg>* maps = mod->getMaps();

	for (vector<MapReg>::iterator it = maps->begin(); it != maps->end(); ++it) {
		this->maps.push_back(*it);
	}
}


/**
* Find a map, by name
**/
MapReg * MapRegistry::get(string name)
{
	for (vector<MapReg>::iterator it = maps.begin(); it != maps.end(); ++it) {
		if (it->getName() == name) return &(*it);
	}
	return NULL;
}

// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <list>
#include <confuse.h>
#include "campaign.h"
#include "mod.h"

using namespace std;


// Campaign stage
static cfg_opt_t campaign_stage_opts[] =
{
	// Game stage
	CFG_STR((char*) "map", 0, CFGF_NONE),
	CFG_STR((char*) "gametype", 0, CFGF_NONE),

	// image
	CFG_STR((char*) "image", 0, CFGF_NONE),
	CFG_INT((char*) "time", 5000, CFGF_NONE),

	CFG_END()
};

/**
* Config file opts
**/
cfg_opt_t campaign_opts[] =
{
	CFG_STR((char*) "name", 0, CFGF_NONE),
	CFG_STR((char*) "title", 0, CFGF_NONE),
	CFG_SEC((char*) "stage", campaign_stage_opts, CFGF_MULTI),
	CFG_END()
};


/**
* Set up
**/
Campaign::Campaign()
{
	this->stages = new vector<CampaignStage*>();
}


/**
* Tear down
**/
Campaign::~Campaign()
{
	for (vector<CampaignStage*>::iterator it = stages->begin(); it != stages->end(); ++it) {
		delete(*it);
	}
	delete(this->stages);
}


/**
* Item loading function handler
**/
Campaign* loadItemCampaign(cfg_t* cfg_item, Mod* mod)
{
	Campaign* c = new Campaign();
	c->name = cfg_getstr(cfg_item, "name");
	c->title = cfg_getstr(cfg_item, "title");
	c->st = mod->st;
	c->mod = mod;

	// Load stages
	int num_stages = cfg_size(cfg_item, "stage");
	for (int j = 0; j < num_stages; j++) {
		cfg_t* cfg_sub = cfg_getnsec(cfg_item, "stage", j);

		CampaignStage *stage = new CampaignStage();
		char* tmp;

		// Game stage
		tmp = cfg_getstr(cfg_sub, "map");
		if (tmp != NULL) {
			stage->map = std::string(tmp);

			tmp = cfg_getstr(cfg_sub, "gametype");
			if (tmp == NULL) {
				mod->setLoadErr("Invalid campaign stage, no 'gametype' specified");
				delete(c);
				delete(stage);
				return NULL;
			}
			stage->gametype = std::string(tmp);
		}

		// Image stage
		tmp = cfg_getstr(cfg_sub, "image");
		if (tmp != NULL) {
			stage->image_filename = std::string(tmp);
			stage->image_time = cfg_getint(cfg_sub, "time");
		}

		// Check is valid
		if (stage->map == "" && stage->image_filename == "") {
			mod->setLoadErr("Invalid campaign stage, no 'map' or 'image' specified");
			delete(c);
			delete(stage);
			return NULL;
		}

		c->stages->push_back(stage);
	}

	return c;
}

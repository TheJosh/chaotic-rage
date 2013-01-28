// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include <list>
#include <confuse.h>
#include <zzip/zzip.h>
#include "campaign.h"
#include "../rage.h"

using namespace std;


// Campaign stage
static cfg_opt_t campaign_stage_opts[] =
{
	CFG_STR((char*) "map", 0, CFGF_NONE),
	CFG_STR((char*) "gametype", 0, CFGF_NONE),
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
	for (vector<CampaignStage*>::iterator it = stages->begin(); it != stages->end();) {
		delete(*it);
	}
	delete(this->stages);
}


/**
* Item loading function handler
**/
Campaign* loadItemCampaign(cfg_t* cfg_item, Mod* mod)
{
	Campaign* c;
	CampaignStage stage;
	cfg_t* cfg_sub;
	int j;

	c = new Campaign();
	c->name = cfg_getstr(cfg_item, "name");
	c->title = cfg_getstr(cfg_item, "title");
	c->st = mod->st;
	
	// Load sounds
	int num_sounds = cfg_size(cfg_item, "stage");
	for (j = 0; j < num_sounds; j++) {
		cfg_sub = cfg_getnsec(cfg_item, "stage", j);
		
		CampaignStage *stage = new CampaignStage();
		stage->map = cfg_getstr(cfg_sub, "map");
		stage->gametype = cfg_getstr(cfg_sub, "gametype");

		c->stages->push_back(stage);
	}
	
	return c;
}

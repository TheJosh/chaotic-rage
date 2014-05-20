// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <list>
#include <confuse.h>
#include "../rage.h"


using namespace std;


class Mod;


// A stage of the campaign
// TODO: Should this be a union?
class CampaignStage {
	public:
		// Game stage
		string map;
		string gametype;

		// Image stage
		string image_filename;
		unsigned int image_time;
};

// A campaign; it's just a list of stages, played in order
class Campaign
{
	public:
		string name;
		CRC32 id;

		string title;
		GameState * st;
		Mod* mod;

		vector<CampaignStage*>* stages;

	public:
		Campaign();
		~Campaign();
};


// Config file opts
extern cfg_opt_t campaign_opts [];

// Item loading function handler
Campaign* loadItemCampaign(cfg_t* cfg_item, Mod* mod);

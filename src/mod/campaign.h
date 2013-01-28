// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include <list>
#include "../rage.h"

using namespace std;


class CampaignStage {
	public:
		string map;
		string gametype;
};

class Campaign
{
	public:
		int id;
		string name;
		string title;
		GameState * st;
		
		vector<CampaignStage*>* stages;
		
	public:
		Campaign();
		~Campaign();
};


// Config file opts
extern cfg_opt_t campaign_opts [];

// Item loading function handler
Campaign* loadItemCampaign(cfg_t* cfg_item, Mod* mod);

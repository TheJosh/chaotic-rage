// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <math.h>
#include "rage.h"

using namespace std;


/* Config file definition */
// Wall
static cfg_opt_t wall_opts[] =
{
	CFG_INT((char*) "x", 0, CFGF_NONE),
	CFG_INT((char*) "y", 0, CFGF_NONE),
	CFG_INT((char*) "angle", 0, CFGF_NONE),
	CFG_STR((char*) "type", ((char*)""), CFGF_NONE),
	CFG_END()
};

static cfg_opt_t zone_opts[] =
{
	CFG_INT((char*) "x", 0, CFGF_NONE),
	CFG_INT((char*) "y", 0, CFGF_NONE),
	CFG_INT((char*) "angle", 0, CFGF_NONE),
	CFG_INT((char*) "width", 0, CFGF_NONE),
	CFG_INT((char*) "height", 0, CFGF_NONE),
	CFG_END()
};

// Main config
static cfg_opt_t opts[] =
{
	CFG_SEC((char*) "wall", wall_opts, CFGF_MULTI),
	CFG_SEC((char*) "zone", zone_opts, CFGF_MULTI),
	CFG_END()
};


Area::Area(AreaType * type)
{
	this->type = type;
	
	if (type->model != NULL) {
		this->anim = new AnimPlay(type->model);
	} else {
		this->anim = NULL;
	}
	
	this->health = 10000;
}

Area::~Area()
{
	delete(anim);
}

Map::Map(GameState * st)
{
	this->st = st;
	this->background = NULL;
	this->data = NULL;
}

Map::~Map()
{
	free(this->data);
}


/**
* Load a file (simulated)
*
* @todo rehash for epicenter positioning
**/
int Map::load(string name, Render * render)
{
	Area *a;
	
	this->render = render;
	this->width = 1000;
	this->height = 1000;
	
	// Default area
	a = new Area(this->st->getMod(0)->getAreaType(0));
	a->x = 0;
	a->y = 0;
	a->width = this->width;
	a->height = this->height;
	a->angle = 0;
	this->areas.push_back(a);
	
	a = new Area(this->st->getMod(0)->getAreaType(2));
	a->x = 300;
	a->y = 300;
	a->width = 100;
	a->height = 100;
	a->angle = 22;
	this->areas.push_back(a);
	
	a = new Area(this->st->getMod(0)->getAreaType(2));
	a->x = 150;
	a->y = 170;
	a->width = 100;
	a->height = 100;
	a->angle = 0;
	this->areas.push_back(a);
	
	a = new Area(this->st->getMod(0)->getAreaType(3));
	a->x = 0;
	a->y = 0;
	a->width = 200;
	a->height = 1000;
	a->angle = 2;
	this->areas.push_back(a);
	
	a = new Area(this->st->getMod(0)->getAreaType(3));
	a->x = 400;
	a->y = 30;
	a->width = 200;
	a->height = 30;
	a->angle = 0;
	this->areas.push_back(a);
	
	
	// Prep the data surface
	// We don't actually use this at the moment, but it may be used later for audio stuff
	// to allow us to know the surface we are currently walking over
	// TODO: get it to actually do something useful here
	this->data = (data_pixel*) malloc(width * height* sizeof(data_pixel));
	
	for (int x = 0; x < this->width; x++) {
		for (int y = 0; y < this->height; y++) {
			this->data[x * this->height + y].type = 0;
			this->data[x * this->height + y].hp = 100;
		}
	}
	
	
	Mod * mod = new Mod(st, "maps/test/");
	this->background = this->render->loadSprite("background.jpg", mod);
	
	{
		cfg_t *cfg, *cfg_sub;
		int num_types, j;
		
		char *buffer = mod->loadText("map.conf");
		if (buffer == NULL) {
			return 0;
		}
		
		cfg = cfg_init(opts, CFGF_NONE);
		cfg_parse_buf(cfg, buffer);
		
		free(buffer);
		
		// Walls
		num_types = cfg_size(cfg, "wall");
		for (j = 0; j < num_types; j++) {
			cfg_sub = cfg_getnsec(cfg, "wall", j);
			
			string type = cfg_getstr(cfg_sub, "type");
			if (type == "") continue;
			
			Wall * wa = new Wall(this->st->getMod(0)->getWallType(type), this->st);
			
			wa->x = cfg_getint(cfg_sub, "x");
			wa->y = cfg_getint(cfg_sub, "y");
			wa->angle = cfg_getint(cfg_sub, "angle");
			
			this->st->addWall(wa);
		}
		
		// Zones
		num_types = cfg_size(cfg, "zone");
		for (j = 0; j < num_types; j++) {
			cfg_sub = cfg_getnsec(cfg, "zone", j);
			
			Zone * z = new Zone(cfg_getint(cfg_sub, "x"), cfg_getint(cfg_sub, "y"), cfg_getint(cfg_sub, "width"), cfg_getint(cfg_sub, "height"));
			
			z->spawn[FACTION_INDIVIDUAL] = 1;
			
			this->zones.push_back(z);
		}
		
	}
	
	return 1;
}


/**
* Gets a data pixel
**/
data_pixel Map::getDataAt(int x, int y)
{
	data_pixel ret;
	ret.type = NULL;
	ret.hp = 0;
	
	if (x < 0) return ret;
	if (y < 0) return ret;
	if (x >= this->width) return ret;
	if (x >= this->height) return ret;
	
	return this->data[x * this->height + y];
}

/**
* Sets a data to a specific health value
**/
void Map::setDataHP(int x, int y, int newhp)
{
	if (x < 0) return;
	if (y < 0) return;
	if (x >= this->width) return;
	if (x >= this->height) return;
	
	if (newhp <= 0) {
		AreaType *at = this->data[x * this->height + y].type;
		
		this->data[x * this->height + y].hp = 0;
		this->data[x * this->height + y].type = at->ground_type;
		
		this->render->clearPixel(x, y);
		
		return;
	}
	
	this->data[x * this->height + y].hp = newhp;
}


/**
* Finds a spawn zone for a specific faction.
* Returns NULL if none are found.
* @todo Choose one randomly instead of grabbing the first one
**/
Zone * Map::getSpawnZone(Faction f)
{
	int num = 0;
	Zone * candidates[20];
	
	for (unsigned int i = 0; i < this->zones.size(); i++) {
		if (this->zones[i]->spawn[f] == 1) {
			candidates[num++] = this->zones[i];
			if (num == 20) break;
		}
	}
	
	if (num == 0) return NULL;
	
	num = (int) round(getRandom(0, num * 10) / 10.0);	//getRandom seems to have some problems with small values
	
	return candidates[num];
}

/**
* Finds a prison zone for a specific faction.
* Returns NULL if none are found.
* @todo Choose one randomly instead of grabbing the first one
**/
Zone * Map::getPrisonZone(Faction f)
{
	for (unsigned int i = 0; i < this->zones.size(); i++) {
		if (this->zones[i]->prison[f] == 1) return this->zones[i];
	}
	
	return NULL;
}

/**
* Finds a collect zone for a specific faction.
* Returns NULL if none are found.
* @todo Choose one randomly instead of grabbing the first one
**/
Zone * Map::getCollectZone(Faction f)
{
	for (unsigned int i = 0; i < this->zones.size(); i++) {
		if (this->zones[i]->collect[f] == 1) return this->zones[i];
	}
	
	return NULL;
}

/**
* Finds a destination zone for a specific faction.
* Returns NULL if none are found.
* @todo Choose one randomly instead of grabbing the first one
**/
Zone * Map::getDestZone(Faction f)
{
	for (unsigned int i = 0; i < this->zones.size(); i++) {
		if (this->zones[i]->dest[f] == 1) return this->zones[i];
	}
	
	return NULL;
}

/**
* Finds a nearbase zone for a specific faction.
* Returns NULL if none are found.
* @todo Choose one randomly instead of grabbing the first one
**/
Zone * Map::getNearbaseZone(Faction f)
{
	for (unsigned int i = 0; i < this->zones.size(); i++) {
		if (this->zones[i]->nearbase[f] == 1) return this->zones[i];
	}
	
	return NULL;
}



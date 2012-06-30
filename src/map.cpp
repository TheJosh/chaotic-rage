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

// Object
static cfg_opt_t object_opts[] =
{
	CFG_INT((char*) "x", 0, CFGF_NONE),
	CFG_INT((char*) "y", 0, CFGF_NONE),
	CFG_INT((char*) "z", 0, CFGF_NONE),
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
	CFG_INT_LIST((char*) "spawn", 0, CFGF_NONE),
	CFG_END()
};

static cfg_opt_t light_opts[] =
{
	CFG_INT((char*) "type", 0, CFGF_NONE),			// 1 = point, 2 = torch
	
	CFG_INT((char*) "x", 0, CFGF_NONE),
	CFG_INT((char*) "y", 0, CFGF_NONE),
	CFG_INT((char*) "z", 0, CFGF_NONE),
	
	CFG_INT_LIST((char*) "ambient", 0, CFGF_NONE),
	CFG_INT_LIST((char*) "diffuse", 0, CFGF_NONE),
	CFG_INT_LIST((char*) "specular", 0, CFGF_NONE),
	
	CFG_END()
};

// Main config
static cfg_opt_t opts[] =
{
	CFG_SEC((char*) "wall", wall_opts, CFGF_MULTI),
	CFG_SEC((char*) "object", object_opts, CFGF_MULTI),
	CFG_SEC((char*) "zone", zone_opts, CFGF_MULTI),
	CFG_SEC((char*) "light", light_opts, CFGF_MULTI),
	
	CFG_INT((char*) "width", 0, CFGF_NONE),
	CFG_INT((char*) "height", 0, CFGF_NONE),
	
	CFG_END()
};


Area::Area(FloorType * type)
{
	this->type = type;
}

Area::~Area()
{
}


Light::Light(unsigned int type)
{
	this->type = type;
	this->ambient[0] = this->ambient[1] = this->ambient[2] = this->ambient[3] = 0.0;
	this->diffuse[0] = this->diffuse[1] = this->diffuse[2] = this->diffuse[3] = 0.0;
	this->specular[0] = this->specular[1] = this->specular[2] = this->specular[3] = 0.0;
}

Light::~Light()
{
}

void Light::setAmbient(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	this->ambient[0] = r / 255.0;
	this->ambient[1] = g / 255.0;
	this->ambient[2] = b / 255.0;
	this->ambient[3] = a / 255.0;
}

void Light::setDiffuse(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	this->diffuse[0] = r / 255.0;
	this->diffuse[1] = g / 255.0;
	this->diffuse[2] = b / 255.0;
	this->diffuse[3] = a / 255.0;
}

void Light::setSpecular(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	this->specular[0] = r / 255.0;
	this->specular[1] = g / 255.0;
	this->specular[2] = b / 255.0;
	this->specular[3] = a / 255.0;
}


Map::Map(GameState * st)
{
	this->st = st;
	this->background = NULL;
}

Map::~Map()
{
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
	this->width = 2000;
	this->height = 2000;
	

	// Default area
	// TODO: Flooring support in the map
	a = new Area(this->st->mm->getFloorType("sandy"));
	a->x = 0;
	a->y = 0;
	a->width = this->width;
	a->height = this->height;
	a->angle = 0;
	this->areas.push_back(a);
	
	
	string filename = "maps/";
	filename.append(name);
	Mod * mod = new Mod(st, filename);
	
	this->background = this->render->loadSprite("background.jpg", mod);
	
	{
		SpritePtr heightmap = this->render->loadSprite("heightmap.png", mod);
		this->render->loadHeightmap(heightmap);
		this->render->freeSprite(heightmap);
	}
	
	{
		cfg_t *cfg, *cfg_sub;
		int num_types, j, k;
		
		char *buffer = mod->loadText("map.conf");
		if (buffer == NULL) {
			return 0;
		}
		
		cfg = cfg_init(opts, CFGF_NONE);
		cfg_parse_buf(cfg, buffer);
		
		free(buffer);
		
		
		this->width = cfg_getint(cfg, "width");
		this->height = cfg_getint(cfg, "height");
		if (this->width == 0 or this->height == 0) return 0;
		
		
		// Walls
		num_types = cfg_size(cfg, "wall");
		for (j = 0; j < num_types; j++) {
			cfg_sub = cfg_getnsec(cfg, "wall", j);
			
			string type = cfg_getstr(cfg_sub, "type");
			if (type.empty()) continue;
			
			WallType *wt = this->st->mm->getWallType(type);
			if (wt == NULL) reportFatalError("Unable to load map; missing or invalid wall type '" + type + "'");

			Wall * wa = new Wall(
				wt,
				this->st,
				cfg_getint(cfg_sub, "x"),
				cfg_getint(cfg_sub, "y"),
				1,
				cfg_getint(cfg_sub, "angle")
			);
			
			this->st->addWall(wa);
		}
		
		// Objects
		num_types = cfg_size(cfg, "object");
		for (j = 0; j < num_types; j++) {
			cfg_sub = cfg_getnsec(cfg, "object", j);
			
			string type = cfg_getstr(cfg_sub, "type");
			if (type.empty()) continue;
			
			ObjectType *ot = this->st->mm->getObjectType(type);
			if (ot == NULL) reportFatalError("Unable to load map; missing or invalid object type '" + type + "'");
			
			Object * ob = new Object(ot, this->st, cfg_getint(cfg_sub, "x"), cfg_getint(cfg_sub, "y"), cfg_getint(cfg_sub, "z"));
			
			ob->angle = cfg_getint(cfg_sub, "angle");
			
			this->st->addObject(ob);
		}

		// Zones
		num_types = cfg_size(cfg, "zone");
		for (j = 0; j < num_types; j++) {
			cfg_sub = cfg_getnsec(cfg, "zone", j);
			
			Zone * z = new Zone(cfg_getint(cfg_sub, "x"), cfg_getint(cfg_sub, "y"), cfg_getint(cfg_sub, "width"), cfg_getint(cfg_sub, "height"));
			
			int num_spawn = cfg_size(cfg_sub, "spawn");
			for (k = 0; k < num_spawn; k++) {
				int f = cfg_getnint(cfg_sub, "spawn", k);
				if (f < FACTION_INDIVIDUAL || f > FACTION_COMMON) continue;
				z->spawn[f] = 1;
			}
			
			this->zones.push_back(z);
		}
		
		// Lights
		num_types = cfg_size(cfg, "light");
		for (j = 0; j < num_types; j++) {
			int num;
			
			cfg_sub = cfg_getnsec(cfg, "light", j);
			
			Light * l = new Light(cfg_getint(cfg_sub, "type"));
			
			l->x = cfg_getint(cfg_sub, "x");
			l->y = cfg_getint(cfg_sub, "y");
			l->z = cfg_getint(cfg_sub, "z");
			
			num = cfg_size(cfg_sub, "ambient");
			if (num == 4) {
				l->setAmbient(
					cfg_getnint(cfg_sub, "ambient", 0),
					cfg_getnint(cfg_sub, "ambient", 1),
					cfg_getnint(cfg_sub, "ambient", 2),
					cfg_getnint(cfg_sub, "ambient", 3)
				);
			}
			
			num = cfg_size(cfg_sub, "diffuse");
			if (num == 4) {
				l->setDiffuse(
					cfg_getnint(cfg_sub, "diffuse", 0),
					cfg_getnint(cfg_sub, "diffuse", 1),
					cfg_getnint(cfg_sub, "diffuse", 2),
					cfg_getnint(cfg_sub, "diffuse", 3)
				);
			}
			
			num = cfg_size(cfg_sub, "specular");
			if (num == 4) {
				l->setSpecular(
					cfg_getnint(cfg_sub, "specular", 0),
					cfg_getnint(cfg_sub, "specular", 1),
					cfg_getnint(cfg_sub, "specular", 2),
					cfg_getnint(cfg_sub, "specular", 3)
				);
			}
			
			this->lights.push_back(l);
		}
		
	}
	
	return 1;
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
	
	num = getRandom(0, num - 1);
	
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



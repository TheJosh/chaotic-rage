// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <map>
#include <math.h>
#include <btBulletDynamicsCommon.h>
#include <glm/glm.hpp>
#include <SDL.h>
#include <SDL_image.h>

#include "map.h"
#include "zone.h"

#include "../util/btStrideHeightfieldTerrainShape.h"
#include "../rage.h"
#include "../mod/confuse_types.h"
#include "../physics_bullet.h"
#include "../render_opengl/animplay.h"
#include "../render/sprite.h"
#include "../render/render_3d.h"
#include "../mod/mod.h"
#include "../mod/mod_manager.h"
#include "../mod/mod_proxy.h"
#include "../mod/vehicletype.h"
#include "../game_state.h"
#include "../game_engine.h"
#include "../entity/object.h"
#include "../entity/vehicle.h"
#include "../entity/helicopter.h"
#include "../entity/wall.h"
#include "../entity/pickup.h"
#include "../util/sdl_util.h"


using namespace std;


class TrainIds {
	public:
		int num;
		int idx;
};

inline bool operator< (const TrainIds& lhs, const TrainIds& rhs)
{
	return ((lhs.num * 100 + lhs.idx) < (rhs.num * 100 + rhs.idx));
}


/* Config file definition */
// Wall
static cfg_opt_t wall_opts[] =
{
	CFG_FLOAT((char*) "x", 0, CFGF_NONE),
	CFG_FLOAT((char*) "y", 0, CFGF_NONE),
	CFG_FLOAT((char*) "angle", 0, CFGF_NONE),
	CFG_STR((char*) "type", ((char*)""), CFGF_NONE),
	CFG_END()
};

// Object
static cfg_opt_t object_opts[] =
{
	CFG_FLOAT((char*) "x", 0, CFGF_NONE),
	CFG_FLOAT((char*) "y", 0, CFGF_NONE),
	CFG_FLOAT((char*) "z", 0, CFGF_NONE),
	CFG_FLOAT((char*) "angle", 0, CFGF_NONE),
	CFG_STR((char*) "type", ((char*)""), CFGF_NONE),
	CFG_END()
};

// Pickup
static cfg_opt_t pickup_opts[] =
{
	CFG_FLOAT((char*) "x", 0, CFGF_NONE),
	CFG_FLOAT((char*) "y", 0, CFGF_NONE),
	CFG_FLOAT((char*) "z", 0, CFGF_NONE),
	CFG_STR((char*) "type", ((char*)""), CFGF_NONE),
	CFG_END()
};

// Vehicle
static cfg_opt_t vehicle_opts[] =
{
	CFG_FLOAT((char*) "x", 0, CFGF_NONE),
	CFG_FLOAT((char*) "y", 0, CFGF_NONE),
	CFG_FLOAT((char*) "z", 0, CFGF_NONE),
	CFG_FLOAT((char*) "angle", 0, CFGF_NONE),
	CFG_STR((char*) "type", ((char*)""), CFGF_NONE),
	CFG_INT((char*) "train-num", 0, CFGF_NONE),
	CFG_INT((char*) "train-idx", 0, CFGF_NONE),
	CFG_END()
};

static cfg_opt_t zone_opts[] =
{
	CFG_FLOAT((char*) "x", 0, CFGF_NONE),
	CFG_FLOAT((char*) "y", 0, CFGF_NONE),
	CFG_FLOAT((char*) "angle", 0, CFGF_NONE),
	CFG_FLOAT((char*) "width", 0, CFGF_NONE),
	CFG_FLOAT((char*) "height", 0, CFGF_NONE),
	CFG_INT_LIST((char*) "spawn", 0, CFGF_NONE),
	CFG_END()
};

static cfg_opt_t light_opts[] =
{
	CFG_INT((char*) "type", 0, CFGF_NONE),			// 1 = point, 2 = torch

	CFG_FLOAT((char*) "x", 0, CFGF_NONE),
	CFG_FLOAT((char*) "y", 0, CFGF_NONE),
	CFG_FLOAT((char*) "z", 0, CFGF_NONE),

	CFG_INT_LIST((char*) "diffuse", 0, CFGF_NONE),
	CFG_INT_LIST((char*) "specular", 0, CFGF_NONE),

	CFG_END()
};

static cfg_opt_t mesh_opts[] =
{
	CFG_FLOAT_LIST((char*) "pos", 0, CFGF_NONE),
	CFG_STR((char*) "model", ((char*)""), CFGF_NONE),
	CFG_END()
};

static cfg_opt_t heightmap_opts[] =
{
	CFG_STR((char*) "data", ((char*)""), CFGF_NONE),
	CFG_STR((char*) "texture", ((char*)""), CFGF_NONE),
	CFG_FLOAT((char*) "scale-z", 4.0f, CFGF_NONE),
	CFG_END()
};

static cfg_opt_t water_opts[] =
{
	CFG_STR((char*) "texture", ((char*)""), CFGF_NONE),
	CFG_FLOAT((char*) "level", 0.0f, CFGF_NONE),			// initial water level
	CFG_FLOAT((char*) "movement", 0.0f, CFGF_NONE),		// total distance between low and high
	CFG_FLOAT((char*) "speed", 0.0f, CFGF_NONE),			// speed (metres/second)
	CFG_END()
};

static cfg_opt_t skybox_opts[] =
{
	CFG_STR((char*) "base", ((char*)""), CFGF_NONE),
	CFG_STR((char*) "ext", ((char*)""), CFGF_NONE),
	CFG_FLOAT_LIST((char*) "size", 0, CFGF_NONE),
	CFG_END()
};

// Main config
static cfg_opt_t opts[] =
{
	CFG_FLOAT((char*) "width", 0, CFGF_NONE),
	CFG_FLOAT((char*) "height", 0, CFGF_NONE),

	CFG_SEC((char*) "wall", wall_opts, CFGF_MULTI),
	CFG_SEC((char*) "vehicle", vehicle_opts, CFGF_MULTI),
	CFG_SEC((char*) "object", object_opts, CFGF_MULTI),
	CFG_SEC((char*) "pickup", pickup_opts, CFGF_MULTI),

	CFG_SEC((char*) "zone", zone_opts, CFGF_MULTI),
	CFG_SEC((char*) "light", light_opts, CFGF_MULTI),
	CFG_SEC((char*) "mesh", mesh_opts, CFGF_MULTI),
	CFG_SEC((char*) "heightmap", heightmap_opts, CFGF_MULTI),
	CFG_SEC((char*) "water", water_opts, CFGF_MULTI),
	CFG_SEC((char*) "skybox", skybox_opts, CFGF_MULTI),

	CFG_INT_LIST((char*) "ambient", 0, CFGF_NONE),

	CFG_END()
};

Light::Light(unsigned int type)
{
	this->type = type;
	this->x = this->y = this->z = 0.0f;
	this->diffuse[0] = this->diffuse[1] = this->diffuse[2] = this->diffuse[3] = 0.0;
	this->specular[0] = this->specular[1] = this->specular[2] = this->specular[3] = 0.0;
}

Light::~Light()
{
}

void Light::setDiffuse(short r, short g, short b, short a)
{
	this->diffuse[0] = r / 255.0f;
	this->diffuse[1] = g / 255.0f;
	this->diffuse[2] = b / 255.0f;
	this->diffuse[3] = a / 255.0f;
}

void Light::setSpecular(short r, short g, short b, short a)
{
	this->specular[0] = r / 255.0f;
	this->specular[1] = g / 255.0f;
	this->specular[2] = b / 255.0f;
	this->specular[3] = a / 255.0f;
}


Map::Map(GameState * st)
{
	this->render = NULL;
	this->st = st;
	this->mod = NULL;
	this->width = 0.0f;
	this->height = 0.0f;
	this->heightmap = NULL;
	this->skybox = NULL;
	this->terrain = NULL;
	this->water = NULL;

	this->ambient[0] = this->ambient[1] = this->ambient[2] = 0.3f;
	this->water_speed = 0.0f;
	this->water_level = 0.0f;
}

Map::~Map()
{
}


/**
* Load a file
*
* TODO: Rehash for epicenter positioning
* TODO: The error reporting in this method is a mess
**/
int Map::load(string name, Render *render, Mod* insideof)
{
	this->render = render;
	this->width = 100;
	this->height = 100;
	this->name = name;


	if (insideof) {
		this->mod = new ModProxy(insideof, "maps/" + name + '/');
	} else {
		string filename = "maps/";
		filename.append(name);
		this->mod = new Mod(st, filename);
	}


	cfg_t *cfg, *cfg_sub;
	int num_types, j, k;

	// Read buffer
	char *buffer = this->mod->loadText("map.conf");
	if (buffer == NULL) {
		reportFatalError("Unable to load map; no configuration file");
	}

	// Parse buffer
	cfg = cfg_init(opts, CFGF_NONE);
	int result = cfg_parse_buf(cfg, buffer);
	free(buffer);

	// Did it work?
	if (result != CFG_SUCCESS) {
		cerr << "Unable to load map; configuration file syntax error\n";
		cfg_free(cfg);
		return 0;
	}

	// Get width and height
	this->width = cfg_getfloat(cfg, "width");
	this->height = cfg_getfloat(cfg, "height");

	if (this->width <= 0.0f || this->height <= 0.0f) {
		cerr << "No width or height set for map.\n";
		cfg_free(cfg);
		return 0;
	}

	// Heightmap
	cfg_sub = cfg_getnsec(cfg, "heightmap", 0);
	if (cfg_sub) {
		this->heightmap = new Heightmap();
		this->heightmap->scale = (float)cfg_getfloat(cfg_sub, "scale-z");

		char* tmp = cfg_getstr(cfg_sub, "data");
		if (tmp == NULL) {
			cerr << "Heightmap config does not have 'data' field set\n";
			cfg_free(cfg);
			return 0;
		}

		if (! this->heightmap->loadIMG(this->mod, std::string(tmp))) {
			cerr << "Failed to load heightmap img " << std::string(tmp) << endl;
			cfg_free(cfg);
			return 0;
		}

		tmp = cfg_getstr(cfg_sub, "texture");
		if (tmp == NULL) {
			cerr << "Heightmap config does not have 'texture' field set\n";
			cfg_free(cfg);
			return 0;
		}

		this->terrain = this->render->loadSprite(std::string(tmp), this->mod);
		if (! this->terrain) {
			cerr << "No terrain image found for map\n";
			cfg_free(cfg);
			return 0;
		}
	} else {
		cerr << "Heightmaps are currently required.\n";
		cfg_free(cfg);
		return 0;
	}

	// Water
	cfg_sub = cfg_getnsec(cfg, "water", 0);
	if (cfg_sub) {
		this->water = this->render->loadSprite("water.png", this->mod);

		if (this->water) {
			this->water_level = (float)cfg_getfloat(cfg_sub, "level");
		}

		float move = (float)cfg_getfloat(cfg_sub, "movement");
		if (move) {
			move /= 2.0f;
			this->water_range.min = this->water_level - move;
			this->water_range.max = this->water_level + move;
			this->water_speed = (float)cfg_getfloat(cfg_sub, "speed") / 1000.0f;		// per sec -> per ms
		} else {
			this->water_speed = 0.0f;
		}
	}

	// Skybox
	cfg_sub = cfg_getnsec(cfg, "skybox", 0);
	if (cfg_sub && this->render->is3D()) {
		Render3D* render3d = static_cast<Render3D*>(this->render);
		this->skybox = render3d->loadCubemap("skybox_", ".jpg", this->mod);
		this->skybox_size = cfg_getvec3(cfg_sub, "size");
	}

	// Ambient lighting
	int num = cfg_size(cfg, "ambient");
	if (num == 3) {
		this->ambient[0] = (float)(cfg_getnint(cfg, "ambient", 0) / 255.0f);
		this->ambient[1] = (float)(cfg_getnint(cfg, "ambient", 1) / 255.0f);
		this->ambient[2] = (float)(cfg_getnint(cfg, "ambient", 2) / 255.0f);
	}

	// Zones
	num_types = cfg_size(cfg, "zone");
	for (j = 0; j < num_types; j++) {
		cfg_sub = cfg_getnsec(cfg, "zone", j);

		Zone * z = new Zone(
			(float)cfg_getfloat(cfg_sub, "x"),
			(float)cfg_getfloat(cfg_sub, "y"),
			(float)cfg_getfloat(cfg_sub, "width"),
			(float)cfg_getfloat(cfg_sub, "height")
		);

		int num_spawn = cfg_size(cfg_sub, "spawn");
		for (k = 0; k < num_spawn; k++) {
			int f = cfg_getnint(cfg_sub, "spawn", k);
			if (f < FACTION_INDIVIDUAL || f >= NUM_FACTIONS) continue;
			z->spawn[f] = 1;
		}

		this->zones.push_back(z);
	}

	// Meshes
	num_types = cfg_size(cfg, "mesh");
	for (j = 0; j < num_types; j++) {
		cfg_sub = cfg_getnsec(cfg, "mesh", j);

		MapMesh * m = new MapMesh();
		m->xform = btTransform(
			btQuaternion(0.0f, 0.0f, 0.0f),
			btVector3(
				(float)cfg_getnfloat(cfg_sub, "pos", 0),
				(float)cfg_getnfloat(cfg_sub, "pos", 1),
				(float)cfg_getnfloat(cfg_sub, "pos", 2)
			)
		);

		char* tmp = cfg_getstr(cfg_sub, "model");
		if (tmp == NULL) continue;

		m->model = new AssimpModel(this->mod, std::string(tmp));

		// TODO: Fix for dedicated server (no Render3D)
		if (! m->model->load(static_cast<Render3D*>(this->render), true)) {
			cerr << "Map model " << tmp << " failed to load.\n";
			continue;
		}

		m->play = new AnimPlay(m->model);

		this->meshes.push_back(m);
	}

	// Lights
	num_types = cfg_size(cfg, "light");
	for (j = 0; j < num_types; j++) {
		int num;

		cfg_sub = cfg_getnsec(cfg, "light", j);

		Light * l = new Light(cfg_getint(cfg_sub, "type"));

		l->x = (float)cfg_getfloat(cfg_sub, "x");
		l->y = (float)cfg_getfloat(cfg_sub, "y");
		l->z = (float)cfg_getfloat(cfg_sub, "z");

		num = cfg_size(cfg_sub, "diffuse");
		if (num == 4) {
			l->setDiffuse(
				(short)cfg_getnint(cfg_sub, "diffuse", 0),
				(short)cfg_getnint(cfg_sub, "diffuse", 1),
				(short)cfg_getnint(cfg_sub, "diffuse", 2),
				(short)cfg_getnint(cfg_sub, "diffuse", 3)
			);
		}

		num = cfg_size(cfg_sub, "specular");
		if (num == 4) {
			l->setSpecular(
				(short)cfg_getnint(cfg_sub, "specular", 0),
				(short)cfg_getnint(cfg_sub, "specular", 1),
				(short)cfg_getnint(cfg_sub, "specular", 2),
				(short)cfg_getnint(cfg_sub, "specular", 3)
			);
		}

		this->lights.push_back(l);
	}


	cfg_free(cfg);
	return 1;
}


/**
* Load the map entities
* THis has to happen after the terrain is created
**/
void Map::loadDefaultEntities()
{
	cfg_t *cfg, *cfg_sub;
	int num_types, j;


	char *buffer = this->mod->loadText("map.conf");
	if (buffer == NULL) {
		return;
	}

	cfg = cfg_init(opts, CFGF_NONE);
	cfg_parse_buf(cfg, buffer);

	free(buffer);


	// Walls
	num_types = cfg_size(cfg, "wall");
	for (j = 0; j < num_types; j++) {
		cfg_sub = cfg_getnsec(cfg, "wall", j);

		string type = cfg_getstr(cfg_sub, "type");
		if (type.empty()) continue;

		WallType *wt = GEng()->mm->getWallType(type);
		if (wt == NULL) reportFatalError("Unable to load map; missing or invalid wall type '" + type + "'");

		Wall * wa = new Wall(
			wt,
			this->st,
			(float)cfg_getfloat(cfg_sub, "x"),
			(float)cfg_getfloat(cfg_sub, "y"),
			1.0f,
			(float)cfg_getfloat(cfg_sub, "angle")
		);

		this->st->addWall(wa);
	}

	// Vehicles
	std::map<TrainIds, Vehicle*> train_nums;
	num_types = cfg_size(cfg, "vehicle");
	for (j = 0; j < num_types; j++) {
		Vehicle * v;
		TrainIds tid;
		cfg_sub = cfg_getnsec(cfg, "vehicle", j);

		string type = cfg_getstr(cfg_sub, "type");
		if (type.empty()) continue;

		VehicleType *vt = GEng()->mm->getVehicleType(type);
		if (vt == NULL) reportFatalError("Unable to load map; missing or invalid vehicle type '" + type + "'");

		if (vt->helicopter) {
			v = new Helicopter(vt, this->st, (float)cfg_getfloat(cfg_sub, "x"), (float)cfg_getfloat(cfg_sub, "y"));
		} else {
			v = new Vehicle(vt, this->st, (float)cfg_getfloat(cfg_sub, "x"), (float)cfg_getfloat(cfg_sub, "y"));
		}

		tid.num = cfg_getint(cfg_sub, "train-num");
		tid.idx = cfg_getint(cfg_sub, "train-idx");
		if (tid.num != 0) {
			train_nums.insert(make_pair(tid, v));
		}

		this->st->addVehicle(v);
	}

	// Hook up trains
	for(std::map<TrainIds, Vehicle*>::iterator it = train_nums.begin(); it != train_nums.end(); ++it) {
		TrainIds srch = TrainIds(it->first);
		srch.idx++;
		if (train_nums.find(srch) == train_nums.end()) continue;
		Vehicle* next = train_nums[srch];
		it->second->trainAttachToNext(next);
	}

	// Objects
	num_types = cfg_size(cfg, "object");
	for (j = 0; j < num_types; j++) {
		cfg_sub = cfg_getnsec(cfg, "object", j);

		string type = cfg_getstr(cfg_sub, "type");
		if (type.empty()) continue;

		ObjectType *ot = GEng()->mm->getObjectType(type);
		if (ot == NULL) reportFatalError("Unable to load map; missing or invalid object type '" + type + "'");

		Object * ob = new Object(ot, this->st, (float)cfg_getfloat(cfg_sub, "x"), (float)cfg_getfloat(cfg_sub, "y"), 1.0f, (float)cfg_getfloat(cfg_sub, "angle"));

		this->st->addObject(ob);
	}

	// Pickups
	num_types = cfg_size(cfg, "pickup");
	for (j = 0; j < num_types; j++) {
		cfg_sub = cfg_getnsec(cfg, "pickup", j);

		string type = cfg_getstr(cfg_sub, "type");
		if (type.empty()) continue;

		PickupType *pt = GEng()->mm->getPickupType(type);
		if (pt == NULL) reportFatalError("Unable to load map; missing or invalid pickup type '" + type + "'");

		Pickup * pu = new Pickup(pt, this->st, (float)cfg_getfloat(cfg_sub, "x"), (float)cfg_getfloat(cfg_sub, "y"), 1.0f);

		this->st->addPickup(pu);
	}

	cfg_free(cfg);
}


/**
* Update map stuff
* Only water level at the moment...
**/
void Map::update(int delta)
{
	if (this->water_speed != 0.0f) {
		this->water_level += (this->water_speed * delta);
		if (this->water_level > this->water_range.max) {
			this->water_speed = -abs(this->water_speed);
		} else if (this->water_level < this->water_range.min) {
			this->water_speed = abs(this->water_speed);
		}
	}
}


/**
* Finds a spawn zone for a specific faction.
* Returns NULL if none are found.
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


/**
* Return a random X co-ord
**/
float Map::getRandomX()
{
	return getRandomf(0, this->width);
}


/**
* Return a random Y co-ord
**/
float Map::getRandomY()
{
	return getRandomf(0, this->height);
}


static bool isPowerOfTwo (unsigned int x)
{
	return ((x != 0) && ((x & (~x + 1)) == x));
}

/**
* Take the heightmap image and turn it into a data array
*
* TODO: Rejig this so we don't use SDL (e.g. prebuild byte array)
**/
bool Heightmap::loadIMG(Mod* mod, string filename)
{
	int nX, nZ;
	Uint8 r,g,b;
	SDL_RWops *rw;
	SDL_Surface *surf;

	// Create rwops
	rw = mod->loadRWops(filename);
	if (rw == NULL) {
		return false;
	}

	// Load image file
	surf = IMG_Load_RW(rw, 0);
	if (surf == NULL) {
		SDL_RWclose(rw);
		return false;
	}

	// Heightmaps need to be powerOfTwo + 1
	if (!isPowerOfTwo(surf->w - 1) || !isPowerOfTwo(surf->h - 1)) {
		SDL_RWclose(rw);
		SDL_FreeSurface(surf);
		return false;
	}

	this->data = new float[surf->w * surf->h];
	this->sx = surf->w;
	this->sz = surf->h;

	for (nZ = 0; nZ < this->sz; nZ++) {
		for (nX = 0; nX < this->sx; nX++) {

			Uint32 pixel = getPixel(surf, nX, nZ);
			SDL_GetRGB(pixel, surf->format, &r, &g, &b);

			this->data[nZ * this->sx + nX] = r / 255.0f * this->scale;

		}
	}

	SDL_RWclose(rw);
	SDL_FreeSurface(surf);
	return true;
}


/**
* Create the "ground" for the map
**/
bool Heightmap::createRigidBody(float mapSX, float mapSZ)
{
	if (this->data == NULL) return false;

	bool flipQuadEdges = false;

	btFasterHeightfieldTerrainShape * groundShape = new btFasterHeightfieldTerrainShape(
		this->sx, this->sz, this->data,
		0,
		0.0f, this->scale,
		1, PHY_FLOAT, flipQuadEdges
	);

	groundShape->setLocalScaling(btVector3(
		mapSX / ((float)this->sx - 1.0f),
		1.0f,
		mapSZ / ((float)this->sz - 1.0f)
	));

	btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(
		btQuaternion(0, 0, 0, 1),
		btVector3(mapSX/2.0f, this->scale/2.0f, mapSZ/2.0f)
	));

	btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(
		0,
		groundMotionState,
		groundShape,
		btVector3(0,0,0)
	);

	this->ground = new btRigidBody(groundRigidBodyCI);
	this->ground->setRestitution(0.f);
	this->ground->setFriction(10.f);

	// Debugging for the terrain without needing to recompile
	if (!debug_enabled("terrain")) {
		this->ground->setCollisionFlags(this->ground->getCollisionFlags()|btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT);
	}

	return true;
}


/**
* Cleanup
**/
Heightmap::~Heightmap()
{
	delete [] data;
	delete ground;
}


/**
* Get a map cell.
* TODO: Remove and support multiple heightmaps
**/
float Map::heightmapGet(int X, int Z)
{
	return heightmap->data[Z * heightmap->sx + X];
}


/**
* Set a map cell. Returns the new value.
* TODO: Remove and support multiple heightmaps
**/
float Map::heightmapSet(int X, int Z, float val)
{
	return heightmap->data[Z * heightmap->sx + X] = val;
}


/**
* Increase (use neg nums to decrease) a map cell. Returns the new value.
* TODO: Remove and support multiple heightmaps
**/
float Map::heightmapAdd(int X, int Z, float amt)
{
	return heightmap->data[Z * heightmap->sx + X] += amt;
}


float Map::heightmapScaleX()
{
	return (float)width / (float)(heightmap->sx-1);
}

float Map::heightmapScaleY()
{
	return this->heightmap->scale;
}

float Map::heightmapScaleZ()
{
	return (float)height / (float)(heightmap->sz-1);
}


/**
* Pre-game map stuff
**/
bool Map::preGame()
{
	// Create heightmap rigid body
	if (! heightmap->createRigidBody(this->width, this->height)) {
		return false;
	}

	// Add to physics
	this->st->physics->addRigidBody(heightmap->ground, CG_TERRAIN);

	// If there is water in the world, we create a water surface
	// It doesn't collide with stuff, it's just so we can detect with a raycast
	if (this->water) {
		btCollisionShape* groundShape = new btBoxShape(btVector3(this->width/2.0f, 10.0f, this->height/2.0f));

		btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(this->width/2.0f, -10.0f + this->water_level, this->height/2.0f)));
		btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(
			0,
			groundMotionState,
			groundShape,
			btVector3(0,0,0)
		);

		btRigidBody *water = new btRigidBody(groundRigidBodyCI);

		water->setRestitution(0.f);
		water->setFriction(10.f);
		this->st->physics->addRigidBody(water, CG_WATER);
	}

	// Load the assimp models into the physics engine
	for (vector<MapMesh*>::iterator it = this->meshes.begin(); it != this->meshes.end(); ++it) {
		MapMesh *mm = (*it);

		// Fill the triangle mesh
		btTriangleMesh *trimesh = new btTriangleMesh(false, false);
		this->fillTriangeMesh(trimesh, mm->play, mm->model, mm->model->rootNode);
		btCollisionShape* meshShape = new btBvhTriangleMeshShape(trimesh, true, true);

		// Create body
		btDefaultMotionState* motionState = new btDefaultMotionState((*it)->xform);
		btRigidBody::btRigidBodyConstructionInfo meshRigidBodyCI(
			0,
			motionState,
			meshShape,
			btVector3(0,0,0)
		);
		btRigidBody *meshBody = new btRigidBody(meshRigidBodyCI);
		meshBody->setRestitution(0.f);
		meshBody->setFriction(10.f);
		this->st->physics->addRigidBody(meshBody, CG_TERRAIN);
	}

	// Add boundry planes which surround the map
	this->st->physics->addRigidBody(this->createBoundaryPlane(btVector3(1.0f, 0.0f, 0.0f), btVector3(0.0f, 0.0f, 0.0f)), CG_TERRAIN);
	this->st->physics->addRigidBody(this->createBoundaryPlane(btVector3(0.0f, 0.0f, 1.0f), btVector3(0.0f, 0.0f, 0.0f)), CG_TERRAIN);
	this->st->physics->addRigidBody(this->createBoundaryPlane(btVector3(-1.0f, 0.0f, 0.0f), btVector3(this->width, 0.0f, this->height)), CG_TERRAIN);
	this->st->physics->addRigidBody(this->createBoundaryPlane(btVector3(0.0f, 0.0f, -1.0f), btVector3(this->width, 0.0f, this->height)), CG_TERRAIN);

	return true;
}


/**
* Fill a triangle mesh with triangles
*
* TODO: It would be better to use btTriangleIndexVertexArray or make AssimpModel implement btStridingMeshInterface
**/
void Map::fillTriangeMesh(btTriangleMesh* trimesh, AnimPlay *ap, AssimpModel *am, AssimpNode *nd)
{
	glm::mat4 transform;
	glm::vec4 a, b, c;
	AssimpMesh* mesh;

	// Grab the transform for this node
	std::map<AssimpNode*, glm::mat4>::iterator local = ap->transforms.find(nd);
	assert(local != ap->transforms.end());
	transform = local->second;

	// Iterate the meshes and add triangles
	for (vector<unsigned int>::iterator it = nd->meshes.begin(); it != nd->meshes.end(); ++it) {
		mesh = am->meshes[(*it)];

		for (vector<AssimpFace>::iterator itt = mesh->faces->begin(); itt != mesh->faces->end(); ++itt) {
			a = transform * mesh->verticies->at((*itt).a);
			b = transform * mesh->verticies->at((*itt).b);
			c = transform * mesh->verticies->at((*itt).c);

			trimesh->addTriangle(btVector3(a.x, a.y, a.z), btVector3(b.x, b.y, b.z), btVector3(c.x, c.y, c.z));
		}
	}

	// Iterate children nodes
	for (vector<AssimpNode*>::iterator it = nd->children.begin(); it != nd->children.end(); ++it) {
		fillTriangeMesh(trimesh, ap, am, (*it));
	}
}


/**
* Cleanup after a game
**/
void Map::postGame()
{
	delete this->heightmap;
	this->heightmap = NULL;

	if (this->terrain != NULL) {
		this->render->freeSprite(this->terrain);
		this->terrain = NULL;
	}

	if (this->skybox != NULL) {
		this->render->freeSprite(this->skybox);
		this->skybox = NULL;
	}
}


/**
* Create a boundary plane (invisible wall)
* There is normally four of these around the edges of the map.
**/
btRigidBody* Map::createBoundaryPlane(const btVector3 &axis, const btVector3 &loc)
{
	btCollisionShape* shape = new btStaticPlaneShape(axis, 0);
	btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1), loc));
	btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(0, motionState, shape, btVector3(0,0,0));
	return new btRigidBody(rigidBodyCI);
}


/**
* Find maps in a given directory and add them to the map registry
**/
void MapRegistry::find(string dir)
{
	maps.push_back(MapReg("therlor_valley", "Therlor Valley"));
	maps.push_back(MapReg("lakeside", "Lakeside"));
	maps.push_back(MapReg("stormy_desert", "Stormy Desert"));
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

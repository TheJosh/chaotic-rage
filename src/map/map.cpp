// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <map>
#include <math.h>
#include <btBulletDynamicsCommon.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SDL.h>
#include <SDL_image.h>

#include "map.h"
#include "zone.h"
#include "heightmap.h"
#include "mesh.h"

#include "../util/btStrideHeightfieldTerrainShape.h"
#include "../rage.h"
#include "../mod/confuse_types.h"
#include "../physics_bullet.h"
#include "../render_opengl/animplay.h"
#include "../render_opengl/light.h"
#include "../render_opengl/texture_splat.h"
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
	CFG_FLOAT((char*) "y", 0, CFGF_NONE),		// TODO: Change to 'z'
	CFG_STR((char*) "type", ((char*)""), CFGF_NONE),
	CFG_END()
};

// Pickup
static cfg_opt_t pickup_opts[] =
{
	CFG_FLOAT((char*) "x", 0, CFGF_NONE),
	CFG_FLOAT((char*) "y", 0, CFGF_NONE),		// TODO: Change to 'z'
	CFG_STR((char*) "type", ((char*)""), CFGF_NONE),
	CFG_INT((char*) "respawn", 1, CFGF_NONE),
	CFG_END()
};

// Vehicle
static cfg_opt_t vehicle_opts[] =
{
	CFG_FLOAT((char*) "x", 0, CFGF_NONE),
	CFG_FLOAT((char*) "y", 0, CFGF_NONE),		// TODO: Change to 'z'
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

static cfg_opt_t texture_splat_opts[] =
{
	CFG_STR((char*) "alphamap", ((char*)""), CFGF_NONE),
	CFG_STR_LIST((char*) "layers", ((char*)""), CFGF_NONE),
	CFG_FLOAT((char*) "scale", 100.0f, CFGF_NONE),
	CFG_END()
};

static cfg_opt_t heightmap_opts[] =
{
	CFG_STR((char*) "data", ((char*)""), CFGF_NONE),
	CFG_STR((char*) "data-type", ((char*)"img"), CFGF_NONE),
	CFG_INT((char*) "data-raw-sx", 0, CFGF_NONE),
	CFG_INT((char*) "data-raw-sz", 0, CFGF_NONE),
	CFG_STR((char*) "normal", ((char*)""), CFGF_NONE),
	CFG_STR((char*) "texture", ((char*)""), CFGF_NONE),
	CFG_SEC((char*) "texture-splat", texture_splat_opts, CFGF_MULTI),
	CFG_FLOAT((char*) "scale-y", 4.0f, CFGF_NONE),
	CFG_FLOAT((char*) "size-x", 0.0f, CFGF_NONE),
	CFG_FLOAT((char*) "size-z", 0.0f, CFGF_NONE),
	CFG_FLOAT_LIST((char*) "pos", 0, CFGF_NONE),
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
	CFG_BOOL((char*) "inf", (cfg_bool_t)true, CFGF_NONE),
	CFG_STR((char*) "daynight", ((char*)""), CFGF_NONE),
	CFG_END()
};

// Main config
static cfg_opt_t opts[] =
{
	CFG_FLOAT((char*) "width", 0, CFGF_NONE),
	CFG_FLOAT((char*) "height", 0, CFGF_NONE),
	CFG_BOOL((char*) "weather", (cfg_bool_t)true, CFGF_NONE),

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


Map::Map(GameState * st)
{
	this->render = NULL;
	this->st = st;
	this->mod = NULL;
	this->width = 0.0f;
	this->height = 0.0f;
	this->weather = false;
	this->skybox = NULL;
	this->skybox_inf = false;

	this->water = NULL;
	this->water_speed = 0.0f;
	this->water_level = 0.0f;
}

Map::~Map()
{
}


static bool isPowerOfTwo (unsigned int x)
{
	return ((x != 0) && ((x & (~x + 1)) == x));
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
	this->weather = cfg_getbool(cfg, "weather");

	if (this->width <= 0.0f || this->height <= 0.0f) {
		cerr << "No width or height set for map.\n";
		cfg_free(cfg);
		return 0;
	}

	// Heightmaps
	num_types = cfg_size(cfg, "heightmap");
	for (j = 0; j < num_types; j++) {
		cfg_sub = cfg_getnsec(cfg, "heightmap", j);

		float scaleY = (float)cfg_getfloat(cfg_sub, "scale-y");

		// Physical size or default to map size
		float sizeX = (float)cfg_getfloat(cfg_sub, "size-x");
		float sizeZ = (float)cfg_getfloat(cfg_sub, "size-z");
		if (sizeX <= 0.0f && sizeZ <= 0.0f) {
			sizeX = this->width;
			sizeZ = this->height;
		}

		// Position or default to center of map
		float posX = (float)cfg_getnfloat(cfg_sub, "pos", 0);
		float posY = (float)cfg_getnfloat(cfg_sub, "pos", 1);
		float posZ = (float)cfg_getnfloat(cfg_sub, "pos", 2);
		if (posX <= 0.0f && posY <= 0.0f && posZ <= 0.0f) {
			posX = this->width/2.0f;
			posY = 0.0f;
			posZ = this->height/2.0f;
		}

		// Get data filename
		char* data_file = cfg_getstr(cfg_sub, "data");
		if (data_file[0] == '\0') {
			cerr << "Heightmap config does not have 'data' field set" << endl;
			cfg_free(cfg);
			return 0;
		}

		char* texture = NULL;
		if (cfg_size(cfg_sub, "texture-splat") == 0) {
			// Load BigTexture
			texture = cfg_getstr(cfg_sub, "texture");
			if (texture == NULL) {
				cerr << "Heightmap config does not have 'texture' field set" << endl;
				cfg_free(cfg);
				return 0;
			}
		}

		// Create heightmap
		Heightmap* heightmap = new Heightmap(sizeX, sizeZ, scaleY, glm::vec3(posX, posY, posZ));

		if (strcmp(cfg_getstr(cfg_sub, "data-type"), "img") == 0) {
			// Load an image (red channel 0 - 255)
			heightmap->data = heightmap->loadIMG(this->mod, std::string(data_file), &heightmap->sx, &heightmap->sz);

		} else if (strcmp(cfg_getstr(cfg_sub, "data-type"), "raw16int") == 0) {
			// Load a raw data file
			heightmap->sx = cfg_getint(cfg_sub, "data-raw-sx");
			heightmap->sz = cfg_getint(cfg_sub, "data-raw-sz");
			heightmap->data = heightmap->loadRAW16(this->mod, std::string(data_file), heightmap->sx, heightmap->sz);

		} else {
			cerr << "Invalid data type; expected 'img' or 'raw16int'" << endl;
			delete heightmap;
			cfg_free(cfg);
			return 0;
		}

		// Heightmaps need to be powerOfTwo + 1
		if (!isPowerOfTwo(heightmap->sx - 1) || !isPowerOfTwo(heightmap->sz - 1)) {
			cerr << "Heightmap must be power-of-two plus one" << endl;
			delete heightmap;
			cfg_free(cfg);
			return 0;
		}

		// Ensure that something was loaded
		if (heightmap->data == NULL) {
			cerr << "Failed to load heightmap data '" << std::string(data_file) << "'" << endl;
			delete heightmap;
			cfg_free(cfg);
			return 0;
		}

		if (cfg_size(cfg_sub, "texture-splat") == 0) {
			// BigTexture
			SpritePtr tex = this->render->loadSprite(std::string(texture), this->mod);
			if (! tex) {
				cerr << "Failed to load heightmap terrain BigTexture '" << std::string(texture) << "'" << endl;
				delete heightmap;
				cfg_free(cfg);
				return 0;
			}
			heightmap->setBigTexture(tex);
		} else {
			// Load texture splat
			TextureSplat* splat = this->loadTextureSplat(cfg_getsec(cfg_sub, "texture-splat"));
			if (splat == NULL) {
				cerr << "Heightmap config does not have valid 'texture-splat' section set" << endl;
				delete heightmap;
				cfg_free(cfg);
				return 0;
			}
			heightmap->setSplatTexture(splat);
		}

		// Load normal data, if specified
		char* normal_file = cfg_getstr(cfg_sub, "normal");
		if (normal_file[0] != '\0') {
			heightmap->normal = this->render->loadSprite(std::string(normal_file), this->mod);
			if (heightmap->normal == NULL) {
				cerr << "Failed to load heightmap normal image '" << std::string(normal_file) << "'" << endl;
				delete heightmap;
				cfg_free(cfg);
				return 0;
			}
		}

		this->heightmaps.push_back(heightmap);
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
		this->skybox_inf = cfg_getbool(cfg_sub, "inf");
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

		// Load model
		char* tmp = cfg_getstr(cfg_sub, "model");
		if (tmp == NULL) continue;
		AssimpModel *model = new AssimpModel(this->mod, std::string(tmp));
		if (model == NULL) continue;

		// TODO: Fix for dedicated server (no Render3D)
		if (! model->load(static_cast<Render3D*>(this->render), true, AssimpLoadMapMesh)) {
			cerr << "Map model " << tmp << " failed to load.\n";
			cfg_free(cfg);
			return 0;
		}

		// Set up position into the transform matrix
		glm::mat4 xform = glm::translate(
			glm::mat4(1.0f),
			glm::vec3(
				(float)cfg_getnfloat(cfg_sub, "pos", 0),
				(float)cfg_getnfloat(cfg_sub, "pos", 1),
				(float)cfg_getnfloat(cfg_sub, "pos", 2)
			)
		);

		MapMesh* m = new MapMesh(xform, model);
		this->meshes.push_back(m);
	}

	cfg_free(cfg);
	return 1;
}


/**
* Load a TextureSplat object from the a config section
**/
TextureSplat* Map::loadTextureSplat(cfg_t *cfg)
{
	unsigned int num_layers, j;
	char* tmpstr;
	TextureSplat* splat;

	splat = new TextureSplat();
	splat->scale = cfg_getfloat(cfg, "scale");

	tmpstr = cfg_getstr(cfg, "alphamap");
	if (tmpstr == NULL) {
		delete splat;
		return NULL;
	}
	splat->alphamap = this->render->loadSprite(std::string(tmpstr), this->mod);

	num_layers = cfg_size(cfg, "layers");
	if (num_layers > TEXTURE_SPLAT_LAYERS) {
		delete splat;
		return NULL;
	}

	for (j = 0; j < num_layers; j++) {
		tmpstr = cfg_getnstr(cfg, "layers", j);
		if (tmpstr == NULL) {
			delete splat;
			return NULL;
		}
		splat->layers[j] = this->render->loadSprite(std::string(tmpstr), this->mod);
	}

	return splat;
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

		Object * ob = new Object(ot, this->st, (float)cfg_getfloat(cfg_sub, "x"), (float)cfg_getfloat(cfg_sub, "y"));

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

		Pickup * pu = new Pickup(pt, this->st, (float)cfg_getfloat(cfg_sub, "x"), (float)cfg_getfloat(cfg_sub, "y"));
		pu->setRespawn(cfg_getint(cfg_sub, "respawn") == 1);

		this->st->addPickup(pu);
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

		this->st->addLight(l);
	}

	// Ambient lighting
	int num = cfg_size(cfg, "ambient");
	if (num == 3) {
		GEng()->render->setAmbient(glm::vec4(
			(float)cfg_getnint(cfg, "ambient", 0) / 255.0f,
			(float)cfg_getnint(cfg, "ambient", 1) / 255.0f,
			(float)cfg_getnint(cfg, "ambient", 2) / 255.0f,
			1.0f
		));
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
* Return a random Z co-ord
**/
float Map::getRandomZ()
{
	return getRandomf(0, this->height);
}


/**
* Pre-game map stuff
**/
bool Map::preGame()
{
	// Create rigid bodies for the heightmaps
	for (vector<Heightmap*>::iterator it = this->heightmaps.begin(); it != this->heightmaps.end(); ++it) {
		btRigidBody* ground = (*it)->createRigidBody();
		if (ground == NULL) {
			return false;
		}
		this->st->physics->addRigidBody(ground, CG_TERRAIN);
	}

	// Create rigid bodies for the map meshes
	for (vector<MapMesh*>::iterator it = this->meshes.begin(); it != this->meshes.end(); ++it) {
		btRigidBody* ground = (*it)->createRigidBody();
		if (ground == NULL) {
			return false;
		}
		this->st->physics->addRigidBody(ground, CG_TERRAIN);
	}
	
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

	// Add boundry planes which surround the map
	this->st->physics->addRigidBody(this->createBoundaryPlane(btVector3(1.0f, 0.0f, 0.0f), btVector3(0.0f, 0.0f, 0.0f)), CG_TERRAIN);
	this->st->physics->addRigidBody(this->createBoundaryPlane(btVector3(0.0f, 0.0f, 1.0f), btVector3(0.0f, 0.0f, 0.0f)), CG_TERRAIN);
	this->st->physics->addRigidBody(this->createBoundaryPlane(btVector3(-1.0f, 0.0f, 0.0f), btVector3(this->width, 0.0f, this->height)), CG_TERRAIN);
	this->st->physics->addRigidBody(this->createBoundaryPlane(btVector3(0.0f, 0.0f, -1.0f), btVector3(this->width, 0.0f, this->height)), CG_TERRAIN);

	return true;
}


/**
* Cleanup after a game
**/
void Map::postGame()
{
	for (vector<Heightmap*>::iterator it = this->heightmaps.begin(); it != this->heightmaps.end(); ++it) {
		delete (*it);
	}

	for (vector<MapMesh*>::iterator it = this->meshes.begin(); it != this->meshes.end(); ++it) {
		delete (*it);
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


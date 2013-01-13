// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <math.h>
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>
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

// Vehicle
static cfg_opt_t vehicle_opts[] =
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

static cfg_opt_t mesh_opts[] =
{
	CFG_FLOAT_LIST((char*) "pos", 0, CFGF_NONE),

	CFG_STR((char*) "mesh", ((char*)""), CFGF_NONE),
	CFG_STR((char*) "texture", ((char*)""), CFGF_NONE),
	
	CFG_END()
};

// Main config
static cfg_opt_t opts[] =
{
	CFG_SEC((char*) "wall", wall_opts, CFGF_MULTI),
	CFG_SEC((char*) "vehicle", vehicle_opts, CFGF_MULTI),
	CFG_SEC((char*) "object", object_opts, CFGF_MULTI),
	CFG_SEC((char*) "zone", zone_opts, CFGF_MULTI),
	CFG_SEC((char*) "light", light_opts, CFGF_MULTI),
	CFG_SEC((char*) "mesh", mesh_opts, CFGF_MULTI),

	CFG_INT((char*) "width", 0, CFGF_NONE),
	CFG_INT((char*) "height", 0, CFGF_NONE),
	
	CFG_FLOAT((char*) "heightmap-z", 4.0f, CFGF_NONE),
	CFG_FLOAT((char*) "water-level", 0.0f, CFGF_NONE),
	
	CFG_INT_LIST((char*) "fog-color", 0, CFGF_NONE),
	CFG_FLOAT((char*) "fog-density", 0.0f, CFGF_NONE),
	
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
	this->heightmap = NULL;
	this->background = NULL;
	this->terrain = NULL;
	this->water = NULL;
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
	this->width = 100;
	this->height = 100;
	this->name = name;
	
	
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
	mod = new Mod(st, filename);
	
	
	cfg_t *cfg, *cfg_sub;
	int num_types, j, k;
	
	char *buffer = mod->loadText("map.conf");
	if (buffer == NULL) {
		reportFatalError("Unable to load map; no configuration file");
	}
	
	cfg = cfg_init(opts, CFGF_NONE);
	cfg_parse_buf(cfg, buffer);
	
	free(buffer);
	
	
	this->width = cfg_getint(cfg, "width");
	this->height = cfg_getint(cfg, "height");
	if (this->width == 0 or this->height == 0) return 0;
	
	this->heightmap_z = cfg_getfloat(cfg, "heightmap-z");
	
	// Water surface
	this->water = this->render->loadSprite("water.png", mod);
	if (this->water) {
		this->water_level = cfg_getfloat(cfg, "water-level");
	}
	
	// Fog
	if (cfg_size(cfg, "fog-color") != 0) {
		this->fog_color[0] = cfg_getnint(cfg, "fog-color", 0) / 255.0f;
		this->fog_color[1] = cfg_getnint(cfg, "fog-color", 1) / 255.0f;
		this->fog_color[2] = cfg_getnint(cfg, "fog-color", 2) / 255.0f;
		this->fog_color[3] = cfg_getnint(cfg, "fog-color", 3) / 255.0f;
		
		this->fog_density = cfg_getfloat(cfg, "fog-density");
	} else {
		this->fog_density = 0.0f;
	}
	
	this->background = this->render->loadSprite("background.jpg", mod);
	if (! this->background) reportFatalError("Unable to load map; no background img");
	
	this->terrain = this->render->loadSprite("terrain.png", mod);
	if (! this->terrain) reportFatalError("Unable to load map; no terran img");
	

	
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
	
	// Meshes
	num_types = cfg_size(cfg, "mesh");
	for (j = 0; j < num_types; j++) {
		cfg_sub = cfg_getnsec(cfg, "mesh", j);
		
		MapMesh * m = new MapMesh();
		m->xform = btTransform(btQuaternion(0.0f, 0.0f, 0.0f, 0.0f), btVector3(cfg_getnfloat(cfg_sub, "pos", 0), cfg_getnfloat(cfg_sub, "pos", 1), cfg_getnfloat(cfg_sub, "pos", 2)));

		{
			string filename = mod->directory;
			filename.append(cfg_getstr(cfg_sub, "mesh"));
			filename.append(".obj");

			WavefrontObj *obj = loadObj(filename);
			if (! obj) reportFatalError("Unable to load map; mesh didn't load");

			m->mesh = obj;
		}

		{
			string filename = cfg_getstr(cfg_sub, "texture");
			filename.append(".png");

			SpritePtr tex = mod->st->render->loadSprite(filename, mod);
			if (! tex) reportFatalError("Unable to load map; mesh texture didn't load");

			m->texture = tex;
		}

		this->meshes.push_back(m);
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
	
	
	char *buffer = mod->loadText("map.conf");
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
	
	// Vehicles
	num_types = cfg_size(cfg, "vehicle");
	for (j = 0; j < num_types; j++) {
		cfg_sub = cfg_getnsec(cfg, "vehicle", j);
		
		string type = cfg_getstr(cfg_sub, "type");
		if (type.empty()) continue;
		
		VehicleType *vt = this->st->mm->getVehicleType(type);
		if (vt == NULL) reportFatalError("Unable to load map; missing or invalid vehicle type '" + type + "'");
		
		Vehicle * v = new Vehicle(vt, this->st, cfg_getint(cfg_sub, "x"), cfg_getint(cfg_sub, "y"));
		
		this->st->addVehicle(v);
	}
	
	// Objects
	num_types = cfg_size(cfg, "object");
	for (j = 0; j < num_types; j++) {
		cfg_sub = cfg_getnsec(cfg, "object", j);
		
		string type = cfg_getstr(cfg_sub, "type");
		if (type.empty()) continue;
		
		ObjectType *ot = this->st->mm->getObjectType(type);
		if (ot == NULL) reportFatalError("Unable to load map; missing or invalid object type '" + type + "'");
		
		Object * ob = new Object(ot, this->st, cfg_getint(cfg_sub, "x"), cfg_getint(cfg_sub, "y"), 1, cfg_getint(cfg_sub, "angle"));
		
		this->st->addObject(ob);
	}
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


/**
* Take the heightmap image and turn it into a data array
**/
void Map::createHeightmapRaw()
{
	int nX, nZ;
	Uint8 r,g,b;
	
	SpritePtr sprite = this->render->loadSprite("heightmap.png", mod);
	if (! sprite) return;;
	
	heightmap = new float[sprite->w * sprite->h];
	heightmap_w = sprite->w;
	heightmap_h = sprite->h;
	
	for ( nZ = 0; nZ < heightmap_h; nZ += 1 ) {
		for ( nX = 0; nX < heightmap_w; nX += 1 ) {
			
			Uint32 pixel = getPixel(sprite->orig, nX, nZ);
			SDL_GetRGB(pixel, sprite->orig->format, &r, &g, &b);
			
			heightmap[nZ * heightmap_w + nX] = r / 255.0f * this->heightmap_z;
			
		}
	}
	
	this->render->freeSprite(sprite);
}


/**
* Get a map cell.
**/
float Map::heightmapGet(int X, int Y)
{
	return heightmap[Y * heightmap_w + X];
}


/**
* Set a map cell. Returns the new value.
**/
float Map::heightmapSet(int X, int Y, float val)
{
	return heightmap[Y * heightmap_w + X] = val;
}


/**
* Increase (use neg nums to decrease) a map cell. Returns the new value.
**/
float Map::heightmapAdd(int X, int Y, float amt)
{
	return heightmap[Y * heightmap_w + X] += amt;
}


float Map::heightmapScaleX()
{
	return (float)width / (float)heightmap_w;
}


float Map::heightmapScaleY()
{
	return (float)height / (float)heightmap_h;
}

float Map::heightmapScaleZ()
{
	return this->heightmap_z;
}


bool Map::preGame()
{
	btRigidBody *ground = this->createGroundBody();
	if (ground == NULL) return false;

	ground->setRestitution(0.f);
	ground->setFriction(10.f);

	this->st->physics->addRigidBody(ground, CG_TERRAIN);


	// If there is water in the world, we create a water surface
	// It doesn't collide with stuff, it's just so we can detect with a raycast
	if (this->water) {
		btCollisionShape* groundShape = new btBoxShape(btVector3(this->st->curr_map->width/2.0f, 10.0f, this->st->curr_map->height/2.0f));
		
		btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(this->st->curr_map->width/2.0f, -10.0f + st->curr_map->water_level, this->st->curr_map->height/2.0f)));
		btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(
			0,
			groundMotionState,
			groundShape,
			btVector3(0,0,0)
		);
		
		ground = new btRigidBody(groundRigidBodyCI);

		ground->setRestitution(0.f);
		ground->setFriction(10.f);
		this->st->physics->addRigidBody(ground, CG_WATER);
	}

	// Meshy goodness
	for (vector<MapMesh*>::iterator it = this->meshes.begin(); it != this->meshes.end(); it++) {
		btTriangleMesh *trimesh = new btTriangleMesh(false, false);

		WavefrontObj *obj = (*it)->mesh;
		int j = 0;
		for (unsigned int i = 0; i < obj->faces.size(); i++) {
			Face * f = &obj->faces.at(i);
			trimesh->addTriangle(
				obj->vertexes.at(f->v1 - 1).toBtVector3(),
				obj->vertexes.at(f->v2 - 1).toBtVector3(),
				obj->vertexes.at(f->v3 - 1).toBtVector3()
			);
		}

		btCollisionShape* mesh = new btBvhTriangleMeshShape(trimesh, true, true);

		btDefaultMotionState* motionState = new btDefaultMotionState((*it)->xform);
		btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(
			0,
			motionState,
			mesh,
			btVector3(0,0,0)
		);
		
		ground = new btRigidBody(groundRigidBodyCI);
		ground->setRestitution(0.f);
		ground->setFriction(10.f);
		this->st->physics->addRigidBody(ground, CG_TERRAIN);
	}
}

void Map::postGame()
{
}


/*
* Create the "ground" for the map
*
* This is a bit crap - the image is loaded twice (1x physics, 1x render)
**/
btRigidBody * Map::createGroundBody()
{
	if (heightmap == NULL) createHeightmapRaw();
	if (heightmap == NULL) return NULL;
	
	bool flipQuadEdges = false;
	
	btHeightfieldTerrainShape * groundShape = new btHeightfieldTerrainShape(
		heightmap_w, heightmap_h, heightmap,
		0,
		0.0f, this->heightmap_z,
		1, PHY_FLOAT, flipQuadEdges
	);
	
	groundShape->setLocalScaling(btVector3(
		heightmapScaleX(),
		1.0f,
		heightmapScaleY()
	));
	
	btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(
		btQuaternion(0, 0, 0, 1),
		btVector3(this->width/2.0f, this->heightmap_z/2.0f, this->height/2.0f)
	));
	
	btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(
		0,
		groundMotionState,
		groundShape,
		btVector3(0,0,0)
	);

	btRigidBody * terrain = new btRigidBody(groundRigidBodyCI);

	terrain->setCollisionFlags(terrain->getCollisionFlags()|btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT);   // Disable debug drawing

	return terrain;
}


/**
* Find maps in a given directory and add them to the map registry
**/
void MapRegistry::find(string dir)
{
	maps.push_back(MapReg("therlor_valley", "Therlor Valley"));
	maps.push_back(MapReg("lakeside", "Lakeside"));
	maps.push_back(MapReg("stormy_desert", "Stormy Desert"));
	maps.push_back(MapReg("arena", "Arena"));
	maps.push_back(MapReg("towers", "Towers"));
	maps.push_back(MapReg("maze", "Maze of the Lost"));
	
	maps.push_back(MapReg("generaltest"));
	maps.push_back(MapReg("tanktest"));
	maps.push_back(MapReg("heighttest"));
}



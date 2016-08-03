// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <glm/glm.hpp>
#include <string>
#include "../rage.h"

using namespace std;


#define RENDER_FRAME_DATA -1
#define RENDER_FRAME_BG -2


class Zone;
class Light;
class Heightmap;
class MapMesh;

class Render;
class Mod;
class RangeF;
class btTriangleMesh;
class btRigidBody;
class Light;


class Map {
	friend class RenderSDL;
	friend class RenderOpenGL;

	private:
		vector<Zone*> zones;
		vector<MapMesh*> meshes;
		Render * render;
		GameState * st;
		Mod * mod;
		string name;

	public:
		float width;
		float height;
		bool weather;

		vector<Heightmap*> heightmaps;

		SpritePtr skybox;
		glm::vec3 skybox_size;
		bool skybox_inf;

		float water_level;
		SpritePtr water;
		RangeF water_range;
		float water_speed;		// distance per ms

	public:
		Map(GameState * st);
		~Map();
		bool preGame();
		void postGame();

	public:
		int load(string name, Render *render, Mod* insideof);
		void loadDefaultEntities();

		bool loadTextureSplat(Heightmap* heightmap, cfg_t *cfg);

		void update(int delta);

		string getName() { return this->name; }

		Zone* getSpawnZone(Faction f);
		Zone* getPrisonZone(Faction f);
		Zone* getCollectZone(Faction f);
		Zone* getDestZone(Faction f);
		Zone* getNearbaseZone(Faction f);
		float getRandomX();
		float getRandomZ();

	private:
		btRigidBody* createBoundaryPlane(const btVector3 &axis, const btVector3 &loc);
};


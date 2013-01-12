// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


#define RENDER_FRAME_DATA -1
#define RENDER_FRAME_BG -2


class Area {
	public:
		int x;
		int y;
		int width;
		int height;
		int angle;
		FloorType * type;
		
	public:
		Area(FloorType * type);
		~Area();
};

class Light {
	public:
		unsigned int type;		// 1, 2 or 3
		float x;
		float y;
		float z;
		float ambient [4];
		float diffuse [4];
		float specular [4];
		
	public:
		Light(unsigned int type);
		~Light();
		
		void setAmbient(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
		void setDiffuse(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
		void setSpecular(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
};


class MapMesh {
	public:
		btTransform xform;
		btRigidBody *body;
		WavefrontObj *mesh;
		SpritePtr texture;
};

class Map {
	friend class RenderSDL;
	friend class RenderOpenGL;
	friend class RenderOpenGLCompat;
	
	private:
		vector<Area*> areas;
		vector<Zone*> zones;
		vector<Light*> lights;
		vector<MapMesh*> meshes;
		Render * render;
		GameState * st;
		Mod * mod;
		string name;
		
	public:
		SpritePtr background;
		SpritePtr terrain;
		int width;
		int height;
		
		float * heightmap;
		int heightmap_w;
		int heightmap_h;
		float heightmap_z;

		float water_level;
		SpritePtr water;
		
		float fog_color[4];
		float fog_density;

	public:
		Map(GameState * st);
		~Map();
		bool preGame();
		void postGame();

	public:
		int load(string name, Render * render);
		void loadDefaultEntities();
		
		string getName() { return this->name; }
		
		Zone * getSpawnZone(Faction f);
		Zone * getPrisonZone(Faction f);
		Zone * getCollectZone(Faction f);
		Zone * getDestZone(Faction f);
		Zone * getNearbaseZone(Faction f);
		float getRandomX();
		float getRandomY();
		
		// todo: support for multiple heightmaps (in different areas)?
		void createHeightmapRaw();
		float heightmapGet(int X, int Y);
		float heightmapSet(int X, int Y, float val);
		float heightmapAdd(int X, int Y, float amt);
		float heightmapScaleX();
		float heightmapScaleY();
		float heightmapScaleZ();
		btRigidBody * createGroundBody();
};


/**
* A single "registered" map
**/
class MapReg {
	friend class MapRegistry;
	
	protected:
		string name;
		string title;
		
	public:
		MapReg() : name(""), title("") {}
		MapReg(string name) : name(name), title(name) {}
		MapReg(string name, string title) : name(name), title(title) {}
};


/**
* The registry of maps
**/
class MapRegistry {
	public:
		vector<MapReg> maps;
		
	public:
		void find(string dir);
		
		string nameAt(int index) { return maps.at(index).name; }
		string titleAt(int index) { return maps.at(index).title; }
		unsigned int size() { return maps.size(); }
};






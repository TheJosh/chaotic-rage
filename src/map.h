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



class Map {
	friend class RenderSDL;
	friend class RenderOpenGL;
	
	private:
		vector<Area*> areas;
		vector<Zone*> zones;
		vector<Light*> lights;
		Render * render;
		GameState * st;
		Mod * mod;
		
	public:
		SpritePtr background;
		int width;
		int height;
		
		float * heightmap;
		int heightmap_w;
		int heightmap_h;
		
	public:
		Map(GameState * st);
		~Map();
		
	public:
		int load(string name, Render * render);
		
		Zone * getSpawnZone(Faction f);
		Zone * getPrisonZone(Faction f);
		Zone * getCollectZone(Faction f);
		Zone * getDestZone(Faction f);
		Zone * getNearbaseZone(Faction f);
		
		float getRandomX();
		float getRandomY();
		
		void createHeightmapRaw();
		float heightmapGet(int X, int Y);
		float heightmapScaleX();
		float heightmapScaleY();
		float heightmapScaleZ();
		btRigidBody * createGroundBody();
};



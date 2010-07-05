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


typedef struct {
	Uint32 type;
	Uint8 hp;
	char pad[3];
} data_pixel;


class Area {
	public:
		int x;
		int y;
		int width;
		int height;
		int angle;
		AreaType* type;
};


class Map {
	private:
		vector<Area*> areas;
		int width;
		int height;
		int colourkey;
		
	public:
		SDL_Surface * ground;
		SDL_Surface * walls;
		data_pixel *data;
		
	public:
		Map();
		~Map();
		
	private:
		SDL_Surface * renderDataSurface();
		
	public:
		int load(string name);
		SDL_Surface * renderFrame(int frame, bool wall);
		data_pixel getDataAt(int x, int y);
		void setDataHP(int x, int y, int newhp);
};

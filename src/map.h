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
		AreaType* type;
};


class Map {
	private:
		vector<Area*> areas;
		int width;
		int height;
		
	public:
		int load(string name);
		SDL_Surface* renderWallFrame(int frame, bool wall);
		SDL_Surface* renderDataSurface();
};

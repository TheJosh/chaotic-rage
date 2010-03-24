#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


class HUD {
	public:
		const static int EVENT_BUBBLE = 1;
		const static int EVENT_PREVENT = 2;
	
	public:
		bool weapon_menu;
		GameState *st;
		
	public:
		HUD();
	
	public:
		void render(SDL_Surface *screen);
		int handleEvent(SDL_Event *event);
		
};

// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

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
		void render(Render * render);
		int handleEvent(SDL_Event *event);
		
	private:
		void tryChangeWeapon(int x, int y);
};

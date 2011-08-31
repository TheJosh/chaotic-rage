// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


class AlertMessage
{
	friend class HUD;
	private:
		string text;
		unsigned int remove_time;
};


class HUD {
	public:
		const static int EVENT_BUBBLE = 1;
		const static int EVENT_PREVENT = 2;
	
	public:
		GameState *st;
		
	private:
		bool weapon_menu;
		bool spawn_menu;
		list<AlertMessage*> msgs;
		
	public:
		HUD(GameState *st);
		void render(RenderOpenGL * render);
		int handleEvent(SDL_Event *event);
		
	public:
		void showSpawnMenu();
		void hideSpawnMenu();

		void addAlertMessage(string text);
		void addAlertMessage(string text1, string text2);
};

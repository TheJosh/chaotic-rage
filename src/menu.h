// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


enum MenuCommand {
	MC_NOTHING = 0,
	MC_SINGLEPLAYER = 1,
	MC_SPLITSCREEN = 2,
	MC_NETWORK = 3,
	MC_SETTINGS = 4,
	MC_QUIT = 5,
};

class MenuItem {
	public:
		string name;
		int x1, x2;
		int y1, y2;
		MenuCommand cmd;
		bool hover;
};


class Menu
{
	private:
		GameState *st;
		RenderOpenGL *render;
		int running;
		vector<MenuItem*> menuitems;
		
		
	private:
		int map;
		vector<string> maps;
		int gametype;
		vector<string> gametypes;
		int viewmode;
		vector<string> viewmodes;
		int unittype;
		vector<string> unittypes;
	
	
	public:
		Menu(GameState *st);
		void doit();
		
	public:
		void menuAdd(string name, int x, int y, MenuCommand cmd);
		void menuRender();
		void menuHover(int x, int y);
		MenuCommand menuClick(int x, int y);
		
	protected:
		void doSingleplayer();
		void doSplitscreen();
		void doNetwork();
		void doSettings();
		void doQuit();
};


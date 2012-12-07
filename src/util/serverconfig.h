// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include <list>
#include "../rage.h"

using namespace std;


class ServerConfig
{
	public:
		string map;
		string gametype;
		unsigned int port;

	public:
		ServerConfig();

	public:
		void load();
		void save();
		void savedefault();

		void initRender(GameState *st);
		void initAudio(GameState *st);
		void initPhysics(GameState *st);
		void initMods(GameState *st);
};

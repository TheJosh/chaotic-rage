// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


class Mod {
	public:
		GameState * st;
		string name;
		string directory;
		
	public:
		char * loadText(string filename);
		SDL_RWops * loadRWops(string filename);
		
		bool loadAreaTypes();
		bool loadUnitClasses();
		bool loadParticleTypes();
		bool loadWeaponTypes();
		
	public:
		Mod(GameState * st, string directory);
};

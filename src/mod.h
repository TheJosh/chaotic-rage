// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


class Mod {
	private:
		vector<AreaType*> * areatypes;
		vector<ParticleType*> * particletypes;
		vector<ParticleGenType*> * pgeneratortypes;
		vector<UnitClass*> * unitclasses;
		vector<WeaponType*> * weapontypes;
		vector<Song*> * songs;
		
	public:
		GameState * st;
		string name;
		string directory;
		
	public:
		char * loadText(string filename);
		SDL_RWops * loadRWops(string filename);
		
	public:
		Mod(GameState * st, string directory);
		
	public:
		bool load();
		
		AreaType * getAreaType(int id);
		UnitClass * getUnitClass(int id);
		ParticleType * getParticleType(int id);
		ParticleGenType * getParticleGenType(int id);
		Song * getSong(int id);
		WeaponType * getWeaponType(int id);
};

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
		vector<AnimModel*> * animmodels;
		vector<FloorType*> * areatypes;
		vector<GameType*> * gametypes;
		vector<ParticleType*> * particletypes;
		vector<ParticleGenType*> * pgeneratortypes;
		vector<Song*> * songs;
		vector<Sound*> * sounds;
		vector<UnitType*> * unitclasses;
		vector<WallType*> * walltypes;
		vector<WeaponType*> * weapontypes;
		
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
		
		AnimModel * getAnimModel(int id);
		FloorType * getFloorType(int id);
		GameType * getGameType(int id);
		ParticleType * getParticleType(int id);
		ParticleGenType * getParticleGenType(int id);
		Song * getSong(int id);
		Sound * getSound(int id);
		UnitType * getUnitType(int id);
		WallType * getWallType(int id);
		WeaponType * getWeaponType(int id);
		
		AnimModel * getAnimModel(string name);
		Sound * getSound(string name);
		WallType * getWallType(string name);
};

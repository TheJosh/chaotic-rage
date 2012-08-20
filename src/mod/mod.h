// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include "../rage.h"

#ifndef O_BINARY
#define O_BINARY 0
#endif


using namespace std;


class Mod {
	private:
		vector<AIType*> * ais;
		vector<AnimModel*> * animmodels;
		vector<FloorType*> * areatypes;
		vector<GameType*> * gametypes;
		vector<ObjectType*> * objecttypes;
		vector<Song*> * songs;
		vector<Sound*> * sounds;
		vector<UnitType*> * unitclasses;
		vector<VehicleType*> * vehicletypes;
		vector<WallType*> * walltypes;
		vector<WeaponType*> * weapontypes;
		
	public:
		GameState * st;
		string name;
		string directory;
		
	public:
		char * loadText(string filename);
		Uint8 * loadBinary(string resname, int * len);
		SDL_RWops * loadRWops(string filename);
		
	public:
		Mod(GameState * st, string directory);
		
	public:
		bool load();
		bool reloadAttrs();
		
		AIType * getAIType(int id);
		AnimModel * getAnimModel(int id);
		FloorType * getFloorType(int id);
		GameType * getGameType(int id);
		ObjectType * getObjectType(int id);
		Song * getSong(int id);
		Sound * getSound(int id);
		UnitType * getUnitType(int id);
		VehicleType * getVehicleType(int id);
		WallType * getWallType(int id);
		WeaponType * getWeaponType(int id);
		
		AIType * getAIType(string name);
		AnimModel * getAnimModel(string name);
		FloorType * getFloorType(string name);
		GameType * getGameType(string name);
		ObjectType * getObjectType(string name);
		UnitType * getUnitType(string name);
		Song * getSong(string name);
		Sound * getSound(string name);
		VehicleType * getVehicleType(string name);
		WallType * getWallType(string name);
		WeaponType * getWeaponType(string name);
		
		void addObjectType(ObjectType * ot);
		
		Song * getRandomSong();

		void getAllGameTypes(vector<GameType*>::iterator * start, vector<GameType*>::iterator * end);
		void getAllUnitTypes(vector<UnitType*>::iterator * start, vector<UnitType*>::iterator * end);
};

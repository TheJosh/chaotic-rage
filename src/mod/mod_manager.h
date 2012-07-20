// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include "../rage.h"

using namespace std;


class ModManager {
	private:
		vector<Mod*> * mods;
		GameState * st;
		
	public:
		ModManager(GameState * st);
		
	public:	
		void addMod(Mod * mod);
		Mod * getMod(string name);
		Mod * getDefaultMod();
		
		bool reloadAttrs();
		
		AIType * getAIType(string name);
		AnimModel * getAnimModel(string name);
		FloorType * getFloorType(string name);
		GameType * getGameType(string name);
		ObjectType * getObjectType(string name);
		UnitType * getUnitType(string name);
		Song * getSong(string name);
		Sound * getSound(string name);
		WallType * getWallType(string name);
		WeaponType * getWeaponType(string name);
		
		Song * getRandomSong();
		
		vector<GameType*> * getAllGameTypes();
		vector<UnitType*> * getAllUnitTypes();
};



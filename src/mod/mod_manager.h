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
		Mod* base;
		Mod* suppl;
		GameState * st;
		
	public:
		ModManager(GameState * st);
		
	public:	
		void addMod(Mod *mod);
		void remMod(Mod *mod);
		void setBase(Mod *mod) { this->base = mod; }
		void setSuppl(Mod *mod) { this->suppl = mod; }
		Mod * getDefaultMod() { return this->base; }
		Mod * getBase() { return this->base; }
		Mod * getSuppl() { return this->suppl; }
		Mod * getSupplOrBase();
		Mod * getMod(string name);
		vector<string> * getLoadedMods();
		vector<string> * getAvailMods();

		bool reloadAttrs();
		
		AIType * getAIType(string name);
		FloorType * getFloorType(string name);
		GameType * getGameType(string name);
		ObjectType * getObjectType(string name);
		PickupType * getPickupType(string name);
		UnitType * getUnitType(string name);
		Song * getSong(string name);
		Sound * getSound(string name);
		VehicleType * getVehicleType(string name);
		WallType * getWallType(string name);
		WeaponType * getWeaponType(string name);
		
		Song * getRandomSong();
		
		vector<GameType*> * getAllGameTypes();
		vector<UnitType*> * getAllUnitTypes();
};


bool loadMods(GameState *st, UIUpdate* ui);




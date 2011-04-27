// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


class FloorTypeDamage {
	public:
		int health;
		AnimModel * model;
};

class FloorType
{
	public:
		// from data file
		string name;
		bool stretch;
		bool wall;
		FloorType * ground_type;
		
		vector <FloorTypeDamage *> damage_models;
		vector <AudioPtr> walk_sounds;
		
		AnimModel * model;
		
		// dynamic
		int id;
		SpritePtr surf;
		
	public:
		FloorType();
};


vector<FloorType*> * loadAllFloorTypes(Mod * mod);

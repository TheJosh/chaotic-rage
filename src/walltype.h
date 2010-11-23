// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


class WallTypeDamage {
	public:
		int health;
		AnimModel * model;
};

class WallType
{
	public:
		// from data file
		string name;
		bool stretch;
		bool wall;
		WallType * ground_type;
		
		vector <WallTypeDamage *> damage_models;
		vector <AudioPtr> walk_sounds;
		
		AnimModel * model;
		
		int check_radius;
		
		// dynamic
		int id;
		SpritePtr surf;
		
	public:
		WallType();
};


vector<WallType*> * loadAllWallTypes(Mod * mod);

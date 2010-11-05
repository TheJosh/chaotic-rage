// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


class AreaTypeDamage {
	public:
		int health;
		AnimModel * model;
};

class AreaType
{
	public:
		// from data file
		string name;
		bool stretch;
		bool wall;
		AreaType * ground_type;
		
		vector <AreaTypeDamage *> damage_models;
		vector <AudioPtr> walk_sounds;
		
		AnimModel * model;
		
		// dynamic
		int id;
		SpritePtr surf;
		
	public:
		AreaType();
};


vector<AreaType*> * loadAllAreaTypes(Mod * mod);

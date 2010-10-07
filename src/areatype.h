// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


class AreaType
{
	public:
		// from data file
		bool stretch;
		bool wall;
		AreaType * ground_type;
		
		vector <AudioPtr> walk_sounds;
		
		// dynamic
		int id;
		SpritePtr surf;
		
	public:
		AreaType();
};


vector<AreaType*> * loadAllAreaTypes(Mod * mod);

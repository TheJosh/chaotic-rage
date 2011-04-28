// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


class FloorType
{
	public:
		// from data file
		string name;
		bool stretch;
		
		// dynamic
		int id;
		SpritePtr texture;
		
	public:
		FloorType();
};


vector<FloorType*> * loadAllFloorTypes(Mod * mod);

// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


class Sound
{
	public:
		// from data file
		string name;
		
		// dynamic
		int id;
		AudioPtr sound;
		
	public:
		Sound();
};


vector<Sound*> * loadAllSounds(Mod * mod);

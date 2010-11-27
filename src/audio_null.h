// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include "rage.h"

using namespace std;


class AudioNull : Audio
{
	public:
		/**
		* Plays sounds
		**/
		virtual void play();
		
		/**
		* Loads a sound
		**/
		virtual AudioPtr loadSound(string filename, Mod * mod);
		
	public:
		virtual void handleEvent(Event * ev);
		
	public:
		AudioNull(GameState * st);
};

// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include <SDL_mixer.h>
#include "rage.h"

using namespace std;

typedef Mix_Chunk* AudioPtr;

class Audio : public EventListener
{
	protected:
		GameState * st;
		
	public:
		/**
		* Plays sounds
		**/
		virtual void play() = 0;
		
		/**
		* Loads a sound
		**/
		virtual AudioPtr loadSound(string filename, Mod * mod) = 0;
		
	public:
		Audio(GameState * st);
};

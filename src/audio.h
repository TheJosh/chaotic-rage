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
		* Plays sounds - This method gets called every ~10ms
		**/
		virtual void play() = 0;
		
		/**
		* Loads a wav file into an AudioPtr
		**/
		virtual AudioPtr loadSound(string filename, Mod * mod) = 0;
		
		/**
		* Plays a sound
		**/
		virtual void playSound(Sound * snd) = 0;
		
		/**
		* Doesn't do anything yet, and the exact way which this will work is still undefined.
		**/
		virtual void stopSound() = 0;
		
	public:
		Audio(GameState * st);
};

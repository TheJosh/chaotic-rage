// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include <SDL_mixer.h>
#include "rage.h"

using namespace std;


class AudioSDLMixer : Audio
{
	private:
		bool audio_start;
		bool audio_stop;		

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
		AudioSDLMixer(GameState * st);
};

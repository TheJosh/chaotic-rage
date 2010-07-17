// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


class AudioSDLMixer : Audio
{
	public:
		/**
		* Plays sounds
		**/
		virtual void play();
		
	public:
		AudioSDLMixer(GameState * st);
};

// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <algorithm>
#include "rage.h"

using namespace std;


/**
* Init SDL_Mixer
**/
AudioNull::AudioNull(GameState * st) : Audio(st)
{
}


/**
* Load a sound
**/
AudioPtr AudioNull::loadSound(string filename, Mod * mod)
{
	AudioPtr sound = new Mix_Chunk();
	return sound;
}


/**
* Handles in-game events
**/
void AudioNull::handleEvent(Event * ev)
{
}


/**
* Plays sounds
**/
void AudioNull::play()
{
}



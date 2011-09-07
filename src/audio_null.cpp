// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <algorithm>
#include "rage.h"

using namespace std;



/**
* We have to return -something-
**/
AudioPtr AudioNull::loadSound(string filename, Mod * mod)
{
	AudioPtr sound = new Mix_Chunk();
	return sound;
}


AudioNull::AudioNull(GameState * st) : Audio(st) {}

void AudioNull::play() {}

int AudioNull::playSound(Sound * snd, bool loop, Entity *e) { return 0; }

void AudioNull::playSong(Song * sng) {}

void AudioNull::stopSound(int id) {}

void AudioNull::stopAll() {}
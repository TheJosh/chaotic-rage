// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <algorithm>
#include <SDL.h>
#include <SDL_mixer.h>
#include "rage.h"

using namespace std;


/**
* Init SDL_Mixer
**/
AudioSDLMixer::AudioSDLMixer(GameState * st) : Audio(st)
{
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
	Mix_AllocateChannels(100);
	this->audio_start = false;
	this->audio_stop = false;
}


/**
* Load a sound
**/
AudioPtr AudioSDLMixer::loadSound(string filename, Mod * mod)
{
	AudioPtr sound;
	SDL_RWops *rw;
	
	DEBUG("Loading sound '%s'.\n", filename.c_str());
	
	rw = mod->loadRWops(filename);
	if (rw == NULL) {
		return NULL;
	}
	
	sound = Mix_LoadWAV_RW(rw, 0);
	if (sound == NULL) {
		return NULL;
	}
	
	SDL_RWclose (rw);
	
	return sound;
}


/**
* Plays sounds - This method gets called every ~10ms
**/
void AudioSDLMixer::play()
{
	// Play sounds
	// You probs should use Audio::playSound instead
	/*for (list<Entity*>::iterator it = st->entities.begin(); it != st->entities.end(); it++) {
		Entity *e = (*it);
		
		Sound * snd = e->getSound();
		if (snd != NULL) {
			Mix_PlayChannel(-1, snd->sound, 0);
		}
	}*/
}


/**
* Something wants to play a song
**/
void AudioSDLMixer::playSong(Song * sg)
{
	Mix_FadeInMusic(sg->music, 0, 1000);
	Mix_VolumeMusic(50);
}


/**
* An entity wants to play a sound
* Returns an int which can be used to halt the sound
**/
int AudioSDLMixer::playSound(Sound * snd, bool loop)
{
	if (snd == NULL) return -1;
	return Mix_PlayChannel(-1, snd->sound, loop ? -1 : 0);
}

/**
* An entity wants to stop a sound
**/
void AudioSDLMixer::stopSound(int id)
{
	if (id == -1) return;
	Mix_HaltChannel(id);
}


/**
* Stops all audio
**/
void AudioSDLMixer::stopAll()
{
	Mix_HaltChannel(-1);
	Mix_HaltMusic();
}



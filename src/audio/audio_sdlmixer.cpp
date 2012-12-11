// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <algorithm>
#include <SDL.h>
#include <SDL_mixer.h>
#include <math.h>
#include "../rage.h"

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

	Mix_VolumeMusic(75);
}


/**
* Load a sound
**/
AudioPtr AudioSDLMixer::loadSound(string filename, Mod * mod)
{
	AudioPtr sound;
	SDL_RWops *rw;
	
	DEBUG("snd", "Loading sound '%s'", filename.c_str());
	
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
	if (Mix_PlayingMusic() == 0) {
		Song *sg = this->st->mm->getRandomSong();
		if (sg != NULL) {
			this->playSong(sg);
			this->st->addHUDMessage(ALL_SLOTS, "Now playing ", sg->name);
		}
	}
}


/**
* Something wants to play a song
**/
void AudioSDLMixer::playSong(Song * sg)
{
	Mix_PlayMusic(sg->music, 1);
}


/**
* An entity wants to play a sound
* Returns an int which can be used to halt the sound
**/
int AudioSDLMixer::playSound(Sound * snd, bool loop, Entity *e)
{
	if (snd == NULL) return -1;
	
	if (st->local_players[0]->p == NULL) return -1;
	
	btTransform trans;
	e->getRigidBody()->getMotionState()->getWorldTransform(trans);
	int x1 = trans.getOrigin().getX();
	int y1 = trans.getOrigin().getY();
	
	st->local_players[0]->p->getRigidBody()->getMotionState()->getWorldTransform(trans);
	int x2 = trans.getOrigin().getX();
	int y2 = trans.getOrigin().getY();
	
	float vol = ((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2));
	if (vol > 25.0 * 25.0) return -1;
	
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



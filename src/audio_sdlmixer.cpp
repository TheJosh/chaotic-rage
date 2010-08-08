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
	this->audio_start = false;
	this->audio_stop = false;
}


/**
* Handles in-game events
**/
void AudioSDLMixer::handleEvent(Event * ev)
{
	if (ev->type == GAME_STARTED) {
		this->audio_start = true;
	}
	
	if (ev->type == GAME_ENDED) {
		this->audio_stop = true;
	}

}


/**
* Plays sounds
**/
void AudioSDLMixer::play()
{
	if (this->audio_start) {
		Song * sg = st->getMod(0)->getSong(0);
		Mix_FadeInMusic(sg->music, 0, 1000);
		Mix_VolumeMusic(50);
		this->audio_start = false;
	}

	if (this->audio_stop) {
		Mix_HaltMusic();
		this->audio_stop = false;	
	}
}



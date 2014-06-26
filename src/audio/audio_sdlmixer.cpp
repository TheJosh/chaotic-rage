// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include "audio_sdlmixer.h"
#include <iostream>
#include "audio.h"
#include "../entity/entity.h"
#include "../entity/player.h"
#include "../game_engine.h"
#include "../game_state.h"
#include "../mod/mod.h"
#include "../mod/mod_manager.h"
#include "../mod/song.h"
#include "../mod/sound.h"
#include "../util/debug.h"

struct SDL_RWops;


using namespace std;


/**
* Init SDL_Mixer
**/
AudioSDLMixer::AudioSDLMixer(GameState * st) : Audio(st)
{
	SDL_InitSubSystem(SDL_INIT_AUDIO);

	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
	Mix_AllocateChannels(100);

	this->audio_start = false;
	this->audio_stop = false;
	this->lastsong = NULL;

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
	if (!sound) {
		cout << "Error: Mix_LoadWAV_RW: " << Mix_GetError() << endl;
	}
	SDL_RWclose(rw);

	return sound;
}


/**
* Plays sounds - This method gets called every ~10ms
**/
void AudioSDLMixer::play()
{
	if (Mix_PlayingMusic() == 0) {
		Song *sg = GEng()->mm->getRandomSong();
		if (sg != NULL && sg != this->lastsong) {
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
	this->lastsong = sg;
}


/**
* An entity wants to play a sound
* Returns an int which can be used to halt the sound
**/
int AudioSDLMixer::playSound(Sound * snd, bool loop, Entity *e)
{
	if (snd == NULL) return -1;

	if (st->local_players[0]->p == NULL) return -1;

	btTransform trans = e->getTransform();
	float x1 = trans.getOrigin().getX();
	float y1 = trans.getOrigin().getY();

	trans = st->local_players[0]->p->getTransform();
	float x2 = trans.getOrigin().getX();
	float y2 = trans.getOrigin().getY();

	float vol = ((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2));
	if (vol > 25.0f * 25.0f) return -1;

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
void AudioSDLMixer::postGame()
{
	Mix_HaltChannel(-1);
	Mix_HaltMusic();
}


/**
* Pause/Resume the music stream
**/
void AudioSDLMixer::toggleMusicPlay()
{
	if (Mix_PausedMusic() != 0) {
		if (this->lastsong != NULL) {
			Mix_ResumeMusic();
			this->st->addHUDMessage(ALL_SLOTS, "Now resuming playing ", this->lastsong->name);
		} else {
			this->play();
		}
	} else {
		Mix_PauseMusic();
		this->st->addHUDMessage(ALL_SLOTS, "Music paused");
	}
}

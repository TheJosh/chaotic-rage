// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include <SDL_mixer.h>
#include "../rage.h"
#include "audio.h"

using namespace std;


class GameState;
class Mod;
class Sound;
class Entity;
class Song;


class AudioSDLMixer : Audio
{
	private:
		bool audio_start;
		bool audio_stop;
		Song *lastsong;

	public:
		/**
		* Plays sounds - This method gets called every ~10ms
		**/
		virtual void play();
		
		/**
		* Loads a wav file into an AudioPtr
		**/
		virtual AudioPtr loadSound(string filename, Mod * mod);
		
		/**
		* Plays a sound
		**/
		virtual int playSound(Sound * snd, bool loop, Entity *e);
		
		/**
		* Plays a song
		**/
		virtual void playSong(Song * sng);

		/**
		* Doesn't do anything yet, and the exact way which this will work is still undefined.
		**/
		virtual void stopSound(int id);
		
		/**
		* Stop everything (sound-wise at least)
		**/
		virtual void postGame();

	public:
		AudioSDLMixer(GameState * st);
};

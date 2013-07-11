// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <SDL.h>
#include <SDL_mixer.h>
#include "../rage.h"

using namespace std;


typedef Mix_Chunk* AudioPtr;

class GameState;


class Audio
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
		* Returns a sound id, or -1 on error
		**/
		virtual int playSound(Sound * snd, bool loop, Entity *e) = 0;
		
		/**
		* Plays a song
		**/
		virtual void playSong(Song * sng) = 0;

		/**
		* Stops a sound based on the sound id
		* If -1 is passed, does nothing
		**/
		virtual void stopSound(int id) = 0;
		
		/**
		* Stop everything (sound-wise at least)
		**/
		virtual void postGame() = 0;

	public:
		Audio(GameState * st);
};

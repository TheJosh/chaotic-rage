// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <SDL_mixer.h>
#include <string>

class Entity;
class GameState;
class Mod;
class Song;


typedef Mix_Chunk* AudioPtr;


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
		virtual AudioPtr loadSound(std::string filename, Mod * mod) = 0;

		/**
		* Plays a sound
		* Returns a sound id, or -1 on error
		**/
		virtual int playSound(AudioPtr snd, bool loop, Entity *e) = 0;

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

		/**
		* Pause/Resume the music stream
		**/
		virtual void toggleMusicPlay() = 0;

	public:
		explicit Audio(GameState * st);
		virtual ~Audio() {}
};

// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


class ParticleType
{
	public:
		// from data file
		string image;
		bool directional;
		int num_frames;
		
		Range begin_speed;
		Range max_speed;
		Range accel;
		Range age;
		Range unit_damage;
		Range wall_damage;
		Range unit_hits;
		Range wall_hits;
		
		AnimModel * model;
		
		// dynamic
		int id;
		vector<SpritePtr> sprites;
		
	public:
		ParticleType();
		~ParticleType();
};

class GenSpew
{
	public:
		ParticleType *pt;
		int angle_range;
		int rate;
		int delay;
		int time;
		int offset;
};

class ParticleGenType
{
	public:
		// from data file
		string name;
		
		// dynamic
		int id;
		vector<GenSpew*> spewers;
};


vector<ParticleType*> * loadAllParticleTypes(Mod * mod);
vector<ParticleGenType*> * loadAllParticleGenTypes(Mod * mod);



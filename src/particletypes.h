// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


class ParticleType : public EntityType
{
	public:
		// from data file
		string image;
		bool directional;
		int num_frames;
		
		Range lin_speed;
		Range lin_accel;
		Range age;
		Range unit_damage;
		Range wall_damage;
		Range unit_hits;
		Range wall_hits;
		
		// dynamic
		int id;
		vector<SpritePtr> sprites;
		
	public:
		ParticleType();
		~ParticleType();
		
	public:
		virtual Entity * spawn(GameState * st);
};

class GenSpew
{
	public:
		EntityType *pt;
		int angle_range;
		int rate;
		int time;
		int offset;
};

class ParticleGenType : public EntityType
{
	public:
		// from data file
		string name;
		
		// dynamic
		int id;
		vector<GenSpew*> spewers;
		
	public:
		virtual Entity * spawn(GameState * st);
};


bool loadAllParticleTypes(Render * render);

ParticleType* getParticleTypeByID(int id);
ParticleGenType* getParticleGenTypeByID(int id);



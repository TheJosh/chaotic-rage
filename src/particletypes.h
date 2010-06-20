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
		
		// dynamic
		int id;
		vector<SDL_Surface*> sprites;
		
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
		int time;
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


bool loadAllParticleTypes();

ParticleType* getParticleTypeByID(int id);
ParticleGenType* getParticleGenTypeByID(int id);



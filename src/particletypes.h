#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


class ParticleType
{
	public:
		// from data file
		string name;
		bool directional;
		int num_frames;
		
		int lin_speed;
		int lin_accel;
		int range;
		int age;
		
		// dynamic
		int id;
		vector<SDL_Surface*> sprites;
		
	public:
		ParticleType();
};

class GenSpew
{
	public:
		ParticleType *pt;
		int angle_range;
		int qty;
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



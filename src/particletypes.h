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
		
		// dynamic
		int id;
		vector<SDL_Surface*> sprites;
		
	public:
		ParticleType();
};


bool loadAllParticleTypes();
ParticleType* getParticleTypeByID(int id);

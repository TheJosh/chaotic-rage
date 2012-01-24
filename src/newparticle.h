// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


class NewParticle {
	public:
		btVector3 pos;
		btVector3 vel;
		float r, g, b;
		int time_death;
};


/**
* Create (num) particles at location (pos) going in direction (vel), with direction randomness of (velrand).
* Colour the particles (r,g,b), and have them survive (lifetime) ms.
**/
void create_particles(GameState * st, unsigned int num, btVector3 pos, btVector3 vel, btVector3 velrand, float r, float g, float b, int lifetime);

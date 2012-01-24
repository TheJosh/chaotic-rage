// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include <math.h>
#include "rage.h"

using namespace std;


void create_particles(GameState * st, unsigned int num, btVector3 pos, btVector3 vel, btVector3 velrand, float r, float g, float b, int lifetime)
{
	NewParticle * p;
	int time_death = st->game_time + lifetime;
	
	for (; num > 0; --num) {
		p = new NewParticle();
		p->pos = pos;
		p->vel = btVector3(
			vel.x() + getRandomf(-velrand.x(), velrand.x()),
			vel.y() + getRandomf(-velrand.y(), velrand.y()),
			vel.z() + getRandomf(-velrand.z(), velrand.z())
		);
		p->r = r;
		p->g = g;
		p->b = b;
		p->time_death = time_death;
		
		st->addNewParticle(p);
	}
}


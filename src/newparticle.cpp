// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include <math.h>
#include "rage.h"

using namespace std;


/**
* Creates particles for a weapon.
* Angles and speeds are randomised. Lifetime is calculated based on the end vector and a constant speed.
*
* TODO: This should have a variable speed as an argument, with the function calculating the age based on the length
*       This will allow the actual (post-rayTest) endpoint to be used, which would be much better
*
* @param num The number of particles to create
* @param start The start location of the particle stream
* @param end The end location of the particle stream
* @param angle_range The range of angles to use for the stream, in degrees
**/
void create_particles_weapon(GameState * st, btVector3 * begin, btVector3 * end, float angle_range)
{
	NewParticle * p;
	
	btVector3 velW = *end - *begin;
	velW /= btScalar(375.f);
	
	int time_death = st->game_time + 375;
	
	angle_range /= 2;
	angle_range = DEG_TO_RAD(angle_range);

	
	btVector3 velP = velW.rotate(btVector3(0.f, 0.f, 1.f), getRandomf(-angle_range, angle_range));
		
	p = new NewParticle();
	p->pos = *begin;
	p->vel = velP;
	p->r = p->g = p->b = .1f;
	p->time_death = time_death;
		
	st->addNewParticle(p);
}


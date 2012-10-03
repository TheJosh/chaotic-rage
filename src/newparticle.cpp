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
**/
void create_particles_weapon(GameState * st, btVector3 * begin, btVector3 * end)
{
	NewParticle * p;
	
	btVector3 velW = *end - *begin;
	velW /= btScalar(375.f);
	
	int time_death = st->game_time + 375;
	
	p = new NewParticle();
	p->pos = *begin;
	p->vel = velW;
	p->r = p->g = p->b = 0.3f;
	p->time_death = time_death;
	
	st->addNewParticle(p);
}


/**
* Creates particles for a flamethrower.
* Angles and speeds are randomised. Lifetime is calculated based on the end vector and a constant speed.
*
* TODO: This should have a variable speed as an argument, with the function calculating the age based on the length
*       This will allow the actual (post-rayTest) endpoint to be used, which would be much better
*
* @param num The number of particles to create
* @param start The start location of the particle stream
* @param end The end location of the particle stream
**/
void create_particles_flamethrower(GameState * st, btVector3 * begin, btVector3 * end)
{
	NewParticle * p;
	
	btVector3 velW = *end - *begin;
	velW /= btScalar(375.f);
	
	int time_death = st->game_time + 375;
	
	p = new NewParticle();
	p->pos = *begin;
	p->vel = velW;
	p->r = getRandomf(0.8f, 1.0f);
	p->g = getRandomf(0.2f, 0.4f);
	p->b = getRandomf(0.0f, 0.1f);
	p->time_death = time_death;
	
	st->addNewParticle(p);
}


/**
* Spray blood in all directions
**/
void create_particles_blood_spray(GameState * st, btVector3 * location, float damage)
{
	NewParticle * p;
	*location += btVector3(0.0f, 0.0f, getRandomf(0.3f, 2.0f));
	btVector3 vel = btVector3(getRandomf(-0.022f, 0.022f), getRandomf(-0.022f, 0.022f), -0.01f);
	
	p = new NewParticle();
	p->pos = *location;
	p->vel = vel;
	p->r = getRandomf(0.8f, 1.0f);
	p->g = getRandomf(0.0f, 0.1f);
	p->b = getRandomf(0.0f, 0.1f);
	p->time_death = st->game_time + 375;
	
	st->addNewParticle(p);
}



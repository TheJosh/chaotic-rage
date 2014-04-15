// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <math.h>
#include "../rage.h"
#include "../game_state.h"
#include "newparticle.h"

using namespace std;


#ifdef USE_SPARK

#include "../spark/SPK.h"

// Gravity, the same for all particles
SPK::Vector3D gravity(0.0f,-0.9f,0.0f);

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
	btVector3 dir = *end - *begin;
	dir.normalize();

	SPK::Model* model = SPK::Model::create(
		SPK::FLAG_RED | SPK::FLAG_GREEN | SPK::FLAG_BLUE | SPK::FLAG_ALPHA, 
		SPK::FLAG_ALPHA,
		SPK::FLAG_RED | SPK::FLAG_GREEN | SPK::FLAG_BLUE
	);

	model->setParam(SPK::PARAM_ALPHA, 1.0f, 0.0f); 
	model->setParam(SPK::PARAM_RED, 0.0f, 1.0f); 
	model->setParam(SPK::PARAM_GREEN, 0.0f, 1.0f);
	model->setParam(SPK::PARAM_BLUE, 0.0f, 1.0f);
	model->setLifeTime(0.5f, 0.7f);

	// Emitter
	SPK::Emitter* emitter = SPK::StraightEmitter::create(SPK::Vector3D(dir.x(), 0.0f, dir.z()));
	emitter->setZone(SPK::Point::create(SPK::Vector3D(begin->x(), begin->y(), begin->z())));
	emitter->setFlow(-1);
	emitter->setTank(25);
	emitter->setForce(40.0f, 50.0f);

	SPK::Group* group = SPK::Group::create(model, 25);
	group->addEmitter(emitter);
	st->addParticleGroup(group);
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
	btVector3 dir = *end - *begin;
	dir.normalize();

	SPK::Model* model = SPK::Model::create(
		SPK::FLAG_RED | SPK::FLAG_GREEN | SPK::FLAG_BLUE | SPK::FLAG_ALPHA, 
		SPK::FLAG_ALPHA,
		SPK::FLAG_RED | SPK::FLAG_GREEN | SPK::FLAG_BLUE
	);

	model->setParam(SPK::PARAM_ALPHA, 1.0f, 0.0f);
	model->setParam(SPK::PARAM_RED, 0.5f, 1.0f);
	model->setParam(SPK::PARAM_GREEN, 0.0f, 0.4f);
	model->setParam(SPK::PARAM_BLUE, 0.0f, 0.1f);
	model->setLifeTime(0.5f, 0.7f);

	SPK::Group* group = SPK::Group::create(model, 25);

	SPK::Emitter* emitter = SPK::SphericEmitter::create(SPK::Vector3D(dir.x(), 0.0f, dir.z()), 0.02f * PI, 0.06f * PI);
	emitter->setZone(SPK::Point::create(SPK::Vector3D(begin->x(), begin->y(), begin->z())));
	emitter->setFlow(-1);
	emitter->setTank(25);
	emitter->setForce(40.0f, 50.0f);
	group->addEmitter(emitter);

	st->addParticleGroup(group);
}


/**
* Spray blood in all directions
**/
void create_particles_blood_spray(GameState * st, btVector3 * location, float damage)
{
	return;
	
	NewParticle * p;
	
	int num = (int)(damage * 0.2f);
	
	for (int i = 0; i <= num; i++) {
		p = new NewParticle();
		
		p->pos = *location + btVector3(0.0f, getRandomf(0.3f, 2.0f), 0.0f);
		p->vel = btVector3(getRandomf(-0.01f, 0.01f), getRandomf(-0.01f, -0.02f), getRandomf(-0.01f, 0.01f));
		p->r = getRandomf(0.7f, 1.0f);
		p->g = getRandomf(0.0f, 0.2f);
		p->b = getRandomf(0.0f, 0.2f);
		p->time_death = st->game_time + 375;
		
		//st->addNewParticle(p);
	}
}


/**
* It's an EXPLOSION!
**/
void create_particles_explosion(GameState * st, btVector3 & location, float damage)
{
	SPK::Model* model;
	SPK::Emitter* emitter;
	SPK::Group* group;
	
	/* Yellow and red fireball */
	model = SPK::Model::create(
		SPK::FLAG_RED | SPK::FLAG_GREEN | SPK::FLAG_BLUE | SPK::FLAG_ALPHA,
		SPK::FLAG_ALPHA,
		SPK::FLAG_RED | SPK::FLAG_GREEN | SPK::FLAG_BLUE
	);
	model->setParam(SPK::PARAM_ALPHA, 0.8f, 1.0f);
	model->setParam(SPK::PARAM_RED, 0.8f, 1.0f);
	model->setParam(SPK::PARAM_GREEN, 0.0f, 0.5f);
	model->setParam(SPK::PARAM_BLUE, 0.0f, 0.1f);
	model->setLifeTime(0.2f, 0.3f);

	// Emitter
	emitter = SPK::RandomEmitter::create();
	emitter->setZone(SPK::Point::create(SPK::Vector3D(location.x(), location.y(), location.z())));
	emitter->setFlow(-1);
	emitter->setTank(4000);
	emitter->setForce(20.0f, 40.0f);

	// Create group
	group = SPK::Group::create(model, 4000);
	group->addEmitter(emitter);
	group->setGravity(gravity);
	st->addParticleGroup(group);
	
	/* Dustsplosion */
	model = SPK::Model::create(
			SPK::FLAG_RED | SPK::FLAG_GREEN | SPK::FLAG_BLUE | SPK::FLAG_ALPHA,
			SPK::FLAG_ALPHA,
			SPK::FLAG_RED | SPK::FLAG_GREEN | SPK::FLAG_BLUE
	);
	model->setParam(SPK::PARAM_ALPHA, 0.2f, 0.0f);
	model->setParam(SPK::PARAM_RED, 0.6f, 0.8f);
	model->setParam(SPK::PARAM_GREEN, 0.6f, 0.8f);
	model->setParam(SPK::PARAM_BLUE, 0.6f, 0.8f);
	model->setLifeTime(1.2f, 8.0f);

	// Emitter
	emitter = SPK::NormalEmitter::create();
	emitter->setZone(SPK::Sphere::create(SPK::Vector3D(location.x(), location.y(), location.z()), 2.0f));
	emitter->setFlow(-1);
	emitter->setTank(2000);
	emitter->setForce(3.0f, 5.0f);

	// Create group
	group = SPK::Group::create(model, 2000);
	group->addEmitter(emitter);
	group->setGravity(gravity);
	group->setFriction(1.0f);
	st->addParticleGroup(group);
}


#else
// No SPARK particles - these become a no-op
void create_particles_weapon(GameState * st, btVector3 * begin, btVector3 * end) {}
void create_particles_flamethrower(GameState * st, btVector3 * begin, btVector3 * end) {}
void create_particles_blood_spray(GameState * st, btVector3 * location, float damage) {}
void create_particles_explosion(GameState * st, btVector3 & location, float damage) {}
#endif

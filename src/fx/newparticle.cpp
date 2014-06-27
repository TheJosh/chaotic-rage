// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include "newparticle.h"


using namespace std;


#ifdef USE_SPARK

#include "../game_state.h"
#include "../rage.h"
#include "../spark/Core/SPK_DEF.h"
#include "../spark/Core/SPK_Emitter.h"
#include "../spark/Core/SPK_Group.h"
#include "../spark/Core/SPK_Model.h"
#include "../spark/Core/SPK_Vector3D.h"
#include "../spark/Extensions/Emitters/SPK_RandomEmitter.h"


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
	SPK::Emitter* emitter = SPK::StraightEmitter::create(SPK::Vector3D(dir.x(), dir.y(), dir.z()));
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

	model->setParam(SPK::PARAM_ALPHA, 0.5f, 0.5f);
	model->setParam(SPK::PARAM_RED, 0.9f, 0.5f);
	model->setParam(SPK::PARAM_GREEN, 0.3f, 0.2f);
	model->setParam(SPK::PARAM_BLUE, 0.1f, 0.0f);
	model->setLifeTime(0.5f, 0.7f);

	SPK::Group* group = SPK::Group::create(model, 100);

	SPK::Emitter* emitter = SPK::SphericEmitter::create(SPK::Vector3D(dir.x(), dir.y(), dir.z()), 0.02f * PI, 0.06f * PI);
	emitter->setZone(SPK::Point::create(SPK::Vector3D(begin->x(), begin->y(), begin->z())));
	emitter->setFlow(-1);
	emitter->setTank(100);
	emitter->setForce(40.0f, 50.0f);
	group->addEmitter(emitter);

	st->addParticleGroup(group);
}


/**
* Spray blood in all directions
**/
void create_particles_blood_spray(GameState * st, const btVector3& location, float damage)
{
	SPK::Model* model;
	SPK::RandomEmitter* emitter;
	SPK::Group* group;

	/* Yellow and red fireball */
	model = SPK::Model::create(
		SPK::FLAG_RED | SPK::FLAG_GREEN | SPK::FLAG_BLUE | SPK::FLAG_ALPHA,
		SPK::FLAG_ALPHA,
		SPK::FLAG_RED | SPK::FLAG_GREEN | SPK::FLAG_BLUE
	);
	model->setParam(SPK::PARAM_ALPHA, 0.8f, 1.0f);
	model->setParam(SPK::PARAM_RED, 0.5f, 1.0f);
	model->setParam(SPK::PARAM_GREEN, 0.0f, 0.1f);
	model->setParam(SPK::PARAM_BLUE, 0.0f, 0.2f);
	model->setLifeTime(0.4f, 0.7f);

	// Falling
	emitter = SPK::RandomEmitter::create();
	emitter->setZone(SPK::Point::create(SPK::Vector3D(location.x(), location.y() - 0.5f, location.z())));
	emitter->setFlow(-1);
	emitter->setTank(damage / 2);
	emitter->setForce(10.0f, 15.0f);

	// Splatting
	emitter = SPK::RandomEmitter::create();
	emitter->setZone(SPK::Point::create(SPK::Vector3D(location.x(), location.y() - 0.5f, location.z())));
	emitter->setFlow(-1);
	emitter->setTank(damage / 2);
	emitter->setForce(10.0f, 15.0f);

	// Create group
	group = SPK::Group::create(model, damage);
	group->addEmitter(emitter);
	group->setGravity(gravity);
	st->addParticleGroup(group);
}


/**
* It's an EXPLOSION!
**/
void create_particles_explosion(GameState * st, const btVector3& location, float damage)
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
void create_particles_blood_spray(GameState * st, const btVector3& location, float damage) {}
void create_particles_explosion(GameState * st, const btVector3& location, float damage) {}
#endif

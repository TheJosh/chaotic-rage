// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include "effects_manager.h"
#include "SPK_TextureQuadRenderer.h"
#include "SPK_ColorQuadRenderer.h"
#include "../rage.h"
#include "../game_state.h"
#include "../game_engine.h"
#include "../spark/SPK.h"
#include "../render/render.h"
#include "../mod/mod.h"
#include "../mod/mod_manager.h"


using namespace std;


// Gravity, the same for all particles
SPK::Vector3D gravity(0.0f, -0.9f, 0.0f);


EffectsManager::EffectsManager(GameState* st)
{
	this->st = st;
	this->setUpCoreEffects();
}


EffectsManager::~EffectsManager()
{
	delete points;
	delete render_blood;
}


/**
* Load the effects models
**/
void EffectsManager::setUpCoreEffects()
{
	points = new SPK::GL::GL2ColorQuadRenderer();
	points->initGLbuffers();
	GEng()->render->addParticleRenderer(points);

	bullets = SPK::Model::create(
		SPK::FLAG_RED | SPK::FLAG_GREEN | SPK::FLAG_BLUE | SPK::FLAG_ALPHA,
		SPK::FLAG_ALPHA,
		SPK::FLAG_RED | SPK::FLAG_GREEN | SPK::FLAG_BLUE
	);
	bullets->setParam(SPK::PARAM_ALPHA, 1.0f, 0.0f);
	bullets->setParam(SPK::PARAM_RED, 0.1f, 0.3f);
	bullets->setParam(SPK::PARAM_GREEN, 0.1f, 0.3f);
	bullets->setParam(SPK::PARAM_BLUE, 0.1f, 0.3f);
	bullets->setLifeTime(0.5f, 0.7f);

	SPK::Model* flames = SPK::Model::create(
		SPK::FLAG_RED | SPK::FLAG_GREEN | SPK::FLAG_BLUE | SPK::FLAG_ALPHA,
		SPK::FLAG_ALPHA,
		SPK::FLAG_RED | SPK::FLAG_GREEN | SPK::FLAG_BLUE
	);
	flames->setParam(SPK::PARAM_ALPHA, 0.5f, 0.5f);
	flames->setParam(SPK::PARAM_RED, 0.9f, 0.5f);
	flames->setParam(SPK::PARAM_GREEN, 0.3f, 0.2f);
	flames->setParam(SPK::PARAM_BLUE, 0.1f, 0.0f);
	flames->setLifeTime(0.5f, 0.7f);

	tex_blood = GEng()->mm->getBase()->getTexture2D("textures/blood.png");

	render_blood = new SPK::GL::GL2InstanceQuadRenderer();
	render_blood->setTexture(tex_blood);
	render_blood->initGLbuffers();
	GEng()->render->addParticleRenderer(render_blood);

	model_blood = SPK::Model::create(
		SPK::FLAG_RED | SPK::FLAG_GREEN | SPK::FLAG_BLUE | SPK::FLAG_ALPHA,
		SPK::FLAG_ALPHA,
		SPK::FLAG_RED | SPK::FLAG_GREEN | SPK::FLAG_BLUE
	);
	model_blood->setParam(SPK::PARAM_ALPHA, 0.8f, 1.0f);
	model_blood->setParam(SPK::PARAM_RED, 0.5f, 1.0f);
	model_blood->setParam(SPK::PARAM_GREEN, 0.0f, 0.1f);
	model_blood->setParam(SPK::PARAM_BLUE, 0.0f, 0.2f);
	model_blood->setLifeTime(0.4f, 0.7f);

	tex_fireball = GEng()->mm->getBase()->getTexture2D("textures/explosion00.png");

	render_fireball = new SPK::GL::GL2InstanceQuadRenderer();
	render_fireball->setTexture(tex_fireball);
	render_fireball->initGLbuffers();
	GEng()->render->addParticleRenderer(render_fireball);
	
	model_fireball = SPK::Model::create(
		SPK::FLAG_RED | SPK::FLAG_GREEN | SPK::FLAG_BLUE | SPK::FLAG_ALPHA,
		SPK::FLAG_ALPHA,
		SPK::FLAG_RED | SPK::FLAG_GREEN | SPK::FLAG_BLUE
	);
	model_fireball->setParam(SPK::PARAM_ALPHA, 0.8f, 1.0f);
	model_fireball->setParam(SPK::PARAM_RED, 0.8f, 1.0f);
	model_fireball->setParam(SPK::PARAM_GREEN, 0.0f, 0.5f);
	model_fireball->setParam(SPK::PARAM_BLUE, 0.0f, 0.1f);
	model_fireball->setLifeTime(0.2f, 0.3f);

	dust = SPK::Model::create(
			SPK::FLAG_RED | SPK::FLAG_GREEN | SPK::FLAG_BLUE | SPK::FLAG_ALPHA,
			SPK::FLAG_ALPHA,
			SPK::FLAG_RED | SPK::FLAG_GREEN | SPK::FLAG_BLUE
	);
	dust->setParam(SPK::PARAM_ALPHA, 0.2f, 0.0f);
	dust->setParam(SPK::PARAM_RED, 0.6f, 0.8f);
	dust->setParam(SPK::PARAM_GREEN, 0.6f, 0.8f);
	dust->setParam(SPK::PARAM_BLUE, 0.6f, 0.8f);
	dust->setLifeTime(1.2f, 8.0f);
}


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
void EffectsManager::weaponBullets(btVector3 * begin, btVector3 * end)
{
	btVector3 dir = *end - *begin;
	dir.normalize();

	// Bullets - emitter
	SPK::Emitter* emitter = SPK::StraightEmitter::create(SPK::Vector3D(dir.x(), dir.y(), dir.z()));
	emitter->setZone(SPK::Point::create(SPK::Vector3D(begin->x(), begin->y(), begin->z())));
	emitter->setFlow(-1);
	emitter->setTank(25);
	emitter->setForce(40.0f, 50.0f);

	// Bullets - group
	SPK::Group* group = SPK::Group::create(bullets, 25);
	group->addEmitter(emitter);
	group->setRenderer(points);
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
void EffectsManager::weaponFlamethrower(btVector3 * begin, btVector3 * end)
{
	btVector3 dir = *end - *begin;
	dir.normalize();

	SPK::Emitter* emitter = SPK::SphericEmitter::create(SPK::Vector3D(dir.x(), dir.y(), dir.z()), 0.02f * PI, 0.06f * PI);
	emitter->setZone(SPK::Point::create(SPK::Vector3D(begin->x(), begin->y(), begin->z())));
	emitter->setFlow(-1);
	emitter->setTank(100);
	emitter->setForce(40.0f, 50.0f);
	
	SPK::Group* group = SPK::Group::create(flames, 100);
	group->addEmitter(emitter);
	group->setRenderer(points);
	st->addParticleGroup(group);
}


/**
* Spray blood in all directions
**/
void EffectsManager::bloodSpray(const btVector3& location, float damage)
{
	SPK::RandomEmitter* emitter;
	SPK::Group* group;

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
	group = SPK::Group::create(model_blood, damage);
	group->addEmitter(emitter);
	group->setGravity(gravity);
	group->setRenderer(render_blood);
	st->addParticleGroup(group);
}


/**
* It's an EXPLOSION!
**/
void EffectsManager::explosion(const btVector3& location, float damage)
{
	SPK::Emitter* emitter;
	SPK::Group* group;

	// Emitter
	emitter = SPK::RandomEmitter::create();
	emitter->setZone(SPK::Point::create(SPK::Vector3D(location.x(), location.y(), location.z())));
	emitter->setFlow(-1);
	emitter->setTank(4000);
	emitter->setForce(20.0f, 40.0f);

	// Create group
	group = SPK::Group::create(model_fireball, 4000);
	group->addEmitter(emitter);
	group->setGravity(gravity);
	group->setRenderer(render_fireball);
	st->addParticleGroup(group);

	// Emitter
	emitter = SPK::NormalEmitter::create();
	emitter->setZone(SPK::Sphere::create(SPK::Vector3D(location.x(), location.y(), location.z()), 2.0f));
	emitter->setFlow(-1);
	emitter->setTank(2000);
	emitter->setForce(3.0f, 5.0f);

	// Create group
	group = SPK::Group::create(dust, 2000);
	group->addEmitter(emitter);
	group->setGravity(gravity);
	group->setFriction(1.0f);
	group->setRenderer(points);
	st->addParticleGroup(group);
}

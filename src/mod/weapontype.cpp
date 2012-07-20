// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include <confuse.h>
#include <zzip/zzip.h>
#include "../rage.h"

using namespace std;


// Sound section
static cfg_opt_t weaponsound_opts[] =
{
	CFG_STR((char*) "sound", 0, CFGF_NONE),
	CFG_INT((char*) "type", 0, CFGF_NONE),
	CFG_END()
};

/**
* Config file opts
**/
cfg_opt_t weapontype_opts[] =
{
	CFG_STR((char*) "name", 0, CFGF_NONE),
	CFG_STR((char*) "title", 0, CFGF_NONE),
	
	CFG_INT((char*) "angle_range", 0, CFGF_NONE),
	CFG_INT((char*) "fire_delay", 0, CFGF_NONE),
	CFG_INT((char*) "reload_delay", 0, CFGF_NONE),
	CFG_INT((char*) "continuous", 0, CFGF_NONE),
	CFG_INT((char*) "magazine_limit", 100, CFGF_NONE),
	CFG_INT((char*) "belt_limit", 1000, CFGF_NONE),
	CFG_FLOAT((char*) "range", 50, CFGF_NONE),
	CFG_FLOAT((char*) "unit_damage", 10, CFGF_NONE),
	CFG_FLOAT((char*) "wall_damage", 10, CFGF_NONE),
	
	CFG_SEC((char*) "sound", weaponsound_opts, CFGF_MULTI),
	CFG_END()
};


/**
* Item loading function handler
**/
WeaponType* loadItemWeaponType(cfg_t* cfg_item, Mod* mod)
{
	WeaponType* wt;
	cfg_t *cfg_sound;
	string filename;
	int j;
	
	wt = new WeaponType();
	wt->name = cfg_getstr(cfg_item, "name");
	wt->title = cfg_getstr(cfg_item, "title");
	wt->st = mod->st;
	
	wt->angle_range = cfg_getint(cfg_item, "angle_range");
	wt->fire_delay = cfg_getint(cfg_item, "fire_delay");
	wt->reload_delay = cfg_getint(cfg_item, "reload_delay");
	wt->continuous = (cfg_getint(cfg_item, "continuous") == 1);
	wt->magazine_limit = cfg_getint(cfg_item, "magazine_limit");
	wt->belt_limit = cfg_getint(cfg_item, "belt_limit");
	wt->range = cfg_getfloat(cfg_item, "range");
	wt->unit_damage = cfg_getfloat(cfg_item, "unit_damage");
	wt->wall_damage = cfg_getfloat(cfg_item, "wall_damage");
	
	
	/// Sounds ///
	int num_sounds = cfg_size(cfg_item, "sound");
	
	// load sounds
	for (j = 0; j < num_sounds; j++) {
		cfg_sound = cfg_getnsec(cfg_item, "sound", j);
		
		WeaponTypeSound* wts = new WeaponTypeSound();
		
		wts->type = cfg_getint(cfg_sound, "type");
		
		char * tmp = cfg_getstr(cfg_sound, "sound");
		if (tmp == NULL) return NULL;
		wts->snd = mod->getSound(tmp);
		
		wt->sounds.push_back(wts);
	}
	
	return wt;
}


/**
* Returns a random sound which matches the specified type.
* If it can't find a sound for that type, return NULL
**/
Sound* WeaponType::getSound(int type)
{
	unsigned int j = 0;
	unsigned int num = 0;
	
	// Find out how many of this time exist
	for (j = 0; j < this->sounds.size(); j++) {
		if (this->sounds.at(j)->type == type) num++;
	}
	
	// Randomly choose one
	num = getRandom(0, num - 1);
	for (j = 0; j < this->sounds.size(); j++) {
		if (this->sounds.at(j)->type == type) {
			if (num == 0) {
				return this->sounds.at(j)->snd;
			}
			num--;
		}
	}
	
	return NULL;
}


WeaponType::WeaponType()
{
}


/**
* Fires a weapon, from a specified Unit
**/
void WeaponType::doFire(Unit * u)
{
	btTransform xform;
	u->body->getMotionState()->getWorldTransform(xform);
	
	// Weapon angle ranges
	int angle = this->angle_range / 2;
	angle = getRandom(-angle, angle);
	
	// TODO: Shouldn't the worldTransform contain the rotation too?
	btQuaternion rot = btQuaternion(btVector3(0.0, 0.0, 1.0), DEG_TO_RAD( ((Player*)u)->mouse_angle + angle ));
	xform.setRotation(rot);
	
	btVector3 forwardDir = xform.getBasis()[1];
	forwardDir.normalize();
	forwardDir *= btScalar(0.f - this->range);		// weapon range
	
	
	DEBUG("weap", "forwardDir is %5.3f %5.3f %5.3f", forwardDir.x(), forwardDir.y(), forwardDir.z());
	DEBUG("weap", "Range is %5.1f", this->range);
	
	btVector3 begin = xform.getOrigin();
	btVector3 end = begin + forwardDir;
	
	DEBUG("weap", "Ray between %5.3f %5.3f %5.3f and %5.3f %5.3f %5.3f", begin.x(), begin.y(), begin.z(), end.x(), end.y(), end.z());
	
	st->addDebugLine(&begin, &end);
	
	
	// Do the rayTest
	btCollisionWorld::ClosestRayResultCallback cb(begin, end);
	this->st->physics->getWorld()->rayTest(begin, end, cb);
	
	if (cb.hasHit()) {
		btRigidBody * body = btRigidBody::upcast(cb.m_collisionObject);
		if (body) {
			Entity* entA = static_cast<Entity*>(body->getUserPointer());
			DEBUG("weap", "Ray hit %p (%p)", body, entA);
			if (entA) {
				this->doHit(entA);
			}
		}
		
	} else {
		DEBUG("weap", "%p Shot; miss", u);
	}
	
	
	// Show the weapon bullets
	create_particles_weapon(u->getGameState(), &begin, &end, 0);
}


/**
* The bullets have hit an entity! do damage!
**/
void WeaponType::doHit(Entity * e)
{
	if (e->klass() == UNIT) {
		((Unit*)e)->takeDamage(this->unit_damage);
		
	} else if (e->klass() == WALL) {
		((Wall*)e)->takeDamage(this->wall_damage);
		
	}
}



// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include <confuse.h>
#include <zzip/zzip.h>
#include "../rage.h"

using namespace std;


static void heightmapCircle(Map* map, int x0, int y0, int radius, float depthadd);



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
	CFG_INT((char*) "type", 1, CFGF_NONE),
	CFG_INT((char*) "fire_delay", 0, CFGF_NONE),
	CFG_INT((char*) "reload_delay", 0, CFGF_NONE),
	CFG_INT((char*) "continuous", 0, CFGF_NONE),
	CFG_INT((char*) "magazine_limit", 100, CFGF_NONE),
	CFG_INT((char*) "belt_limit", 1000, CFGF_NONE),

	// WEAPON_TYPE_RAYCAST
	CFG_INT((char*) "angle_range", 0, CFGF_NONE),
	CFG_FLOAT((char*) "range", 50, CFGF_NONE),
	CFG_FLOAT((char*) "unit_damage", 10, CFGF_NONE),
	CFG_FLOAT((char*) "wall_damage", 10, CFGF_NONE),
	
	// WEAPON_TYPE_DIGDOWN
	CFG_INT((char*) "radius", 2, CFGF_NONE),
	CFG_FLOAT((char*) "depth", -0.1, CFGF_NONE),

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
	int j, type;
	
	type = cfg_getint(cfg_item, "type");

	// The exact class is dependent on the type
	if (type == WEAPON_TYPE_RAYCAST) {
		WeaponRaycast* w = new WeaponRaycast();
		w->angle_range = cfg_getint(cfg_item, "angle_range");
		w->range = cfg_getfloat(cfg_item, "range");
		w->unit_damage = cfg_getfloat(cfg_item, "unit_damage");
		w->wall_damage = cfg_getfloat(cfg_item, "wall_damage");
		wt = w;

	} else if (type == WEAPON_TYPE_DIGDOWN) {
		WeaponDigdown* w = new WeaponDigdown();
		w->radius = cfg_getint(cfg_item, "radius");
		w->depth = cfg_getfloat(cfg_item, "depth");
		wt = w;

	} else if (type == WEAPON_TYPE_FLAMETHROWER) {
		WeaponFlamethrower* w = new WeaponFlamethrower();
		w->angle_range = cfg_getint(cfg_item, "angle_range");
		w->range = cfg_getfloat(cfg_item, "range");
		w->unit_damage = cfg_getfloat(cfg_item, "unit_damage");
		w->wall_damage = cfg_getfloat(cfg_item, "wall_damage");
		wt = w;

	} else {
		return NULL;
	}

	// These settings are common to all weapon types
	wt->name = cfg_getstr(cfg_item, "name");
	wt->title = cfg_getstr(cfg_item, "title");
	wt->st = mod->st;
	
	wt->fire_delay = cfg_getint(cfg_item, "fire_delay");
	wt->reload_delay = cfg_getint(cfg_item, "reload_delay");
	wt->continuous = (cfg_getint(cfg_item, "continuous") == 1);
	wt->magazine_limit = cfg_getint(cfg_item, "magazine_limit");
	wt->belt_limit = cfg_getint(cfg_item, "belt_limit");
	
	// Load sounds
	int num_sounds = cfg_size(cfg_item, "sound");
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
void WeaponRaycast::doFire(Unit * u)
{
	btTransform xform;
	u->body->getMotionState()->getWorldTransform(xform);
	
	// Weapon angle ranges
	int angle = this->angle_range / 2;
	angle = getRandom(-angle, angle);
	btQuaternion rot = xform.getRotation() * btQuaternion(btVector3(0.0, 1.0, 0.0), DEG_TO_RAD(angle));
	xform.setRotation(rot);
	
	// Ray direction
	btVector3 forwardDir = xform.getBasis()[2];
	forwardDir.normalize();
	forwardDir *= btScalar(0 - this->range);
	
	// Ray begin and end points
	btVector3 begin = xform.getOrigin();
	btVector3 end = begin + forwardDir;
	
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
				if (entA->klass() == UNIT) {
					((Unit*)entA)->takeDamage(this->unit_damage);
				} else if (entA->klass() == WALL) {
					((Wall*)entA)->takeDamage(this->wall_damage);
				}
			}
		}
		
	} else {
		DEBUG("weap", "%p Shot; miss", u);
	}
	
	
	// Show the weapon bullets
	create_particles_weapon(u->getGameState(), &begin, &end);
}


/**
* Fires a weapon, from a specified Unit
**/
void WeaponDigdown::doFire(Unit * u)
{
	btTransform xform;
	u->body->getMotionState()->getWorldTransform(xform);
	
	btVector3 begin = xform.getOrigin();
	btVector3 end = begin + btVector3(0.0f, -100.0f, 0.0f);
	
	//st->addDebugLine(&begin, &end);
	
	
	// Do the rayTest
	btCollisionWorld::ClosestRayResultCallback cb(begin, end);
	this->st->physics->getWorld()->rayTest(begin, end, cb);
	
	if (cb.hasHit()) {
		btRigidBody * body = btRigidBody::upcast(cb.m_collisionObject);
		if (body && !body->getUserPointer()) {
			// Hit the ground, lets dig a hole
			Map* map = u->st->curr_map;
			int mapX = begin.x() / map->width * map->heightmap_w;
			int mapY = begin.y() / map->height * map->heightmap_h;

			heightmapCircle(map, mapX, mapY, this->radius, this->depth);

			u->st->render->freeHeightmap();
			u->st->render->loadHeightmap();
		}
	}
}


/**
* Used by the dig/mound weapons
**/
void heightmapCircle(Map* map, int x0, int y0, int radius, float depthadd)
{
	int x, y, d;

	for (y = -radius; y <= radius; y++) {
		for (x = -radius; x <= radius; x++) {
			d = (radius * radius) - ((x * x) + (y * y));
			if (d > 0) {
				// distance in = d
				map->heightmapAdd(x0 + x, y0 + y, depthadd * d * 0.14);
			}
		}
	}
}


/**
* Fires a weapon, from a specified Unit
**/
void WeaponFlamethrower::doFire(Unit * u)
{
	btTransform xform;
	u->body->getMotionState()->getWorldTransform(xform);
	
	// Weapon angle ranges
	int angle = this->angle_range / 2;
	angle = getRandom(-angle, angle);
	btQuaternion rot = xform.getRotation() * btQuaternion(btVector3(0.0, 1.0, 0.0), DEG_TO_RAD(angle));
	xform.setRotation(rot);
	
	// Ray direction
	btVector3 forwardDir = xform.getBasis()[2];
	forwardDir.normalize();
	forwardDir *= btScalar(0 - this->range);
	
	// Ray begin and end points
	btVector3 begin = xform.getOrigin();
	btVector3 end = begin + forwardDir;
	
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
				if (entA->klass() == UNIT) {
					((Unit*)entA)->takeDamage(this->unit_damage);
				} else if (entA->klass() == WALL) {
					((Wall*)entA)->takeDamage(this->wall_damage);
				}
			}
		}
		
	} else {
		DEBUG("weap", "%p Shot; miss", u);
	}
	
	// Show the weapon fire
	create_particles_flamethrower(u->getGameState(), &begin, &end);
}

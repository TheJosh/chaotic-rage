// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include <confuse.h>
#include <zzip/zzip.h>
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include "../rage.h"
#include "ghost_objects.h"

using namespace std;



/**
* Fires a weapon, from a specified Unit
**/
void WeaponAttractor::doFire(Unit *u, btTransform &origin)
{
	btTransform xform = origin;	
	AmmoRound* ar = new AmmoRound(u->getGameState(), xform, this, this->model, u);
	
	WeaponAttractorData* data = new WeaponAttractorData();
	data->destroy_time = u->getGameState()->game_time + this->time;
	data->ghost = create_ghost(xform, this->range);
	ar->data = data;
	
	u->getGameState()->addAmmoRound(ar);
	st->physics->addCollisionObject(data->ghost, CG_AMMO);
}


/**
* Called at regular intervals by the attractor entity
**/
void WeaponAttractor::entityUpdate(AmmoRound *e, int delta)
{
	WeaponAttractorData* data = (WeaponAttractorData*)e->data;
	
	// Remove the attractor after it's too old
	if (data->destroy_time < e->getGameState()->game_time) {
		e->getGameState()->physics->delCollisionObject(data->ghost);
		delete(data);
		e->del = true;
		return;
	}

	// If there is something within range...
	if (check_ghost_triggered(data->ghost)) {
		cout << "In the attractor!\n";
	}
}


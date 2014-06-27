// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <btBulletDynamicsCommon.h>
class btVector3;
class GameState;


class NewParticle
{
	public:
		btVector3 pos;
		btVector3 vel;
		float r, g, b;
		unsigned int time_death;
};


void create_particles_weapon(GameState * st, btVector3 * begin, btVector3 * end);
void create_particles_flamethrower(GameState * st, btVector3 * begin, btVector3 * end);
void create_particles_blood_spray(GameState * st, const btVector3& location, float damage);
void create_particles_explosion(GameState * st, const btVector3& location, float damage);

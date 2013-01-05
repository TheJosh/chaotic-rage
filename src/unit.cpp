// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include <math.h>
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>
#include "rage.h"

using namespace std;


Unit::Unit(UnitType *uc, GameState *st, float x, float y, float z) : Entity(st)
{
	this->uc = uc;
	this->health = uc->begin_health;
	this->slot = 0;
	
	this->weapon = NULL;
	this->firing = false;
	
	this->anim = NULL;
	this->current_state_type = 0;
	this->setState(UNIT_STATE_STATIC);
	
	this->walk_state = uc->getState(UNIT_STATE_WALK);
	this->walk_time = 0;
	
	this->remove_at = 0;
	
	this->lift_obj = NULL;
	this->drive = NULL;
	this->turret_obj = NULL;
	
	this->melee_time = 0;
	this->melee_cooldown = 0;
	
	this->weapon_sound = -1;
	
	this->uts = NULL;
	this->setModifiers(0);



	btTransform xform = btTransform(
		btQuaternion(btVector3(0,0,1),0),
		st->physics->spawnLocation(x, y, 0.9f)
	);

	this->ghost = new btPairCachingGhostObject();
	this->ghost->setWorldTransform(xform);

	btConvexShape* capsule = new btCapsuleShape(0.5f, 0.9f);
	this->ghost->setCollisionShape(capsule);
	this->ghost->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);
	
	btScalar stepHeight = btScalar(0.75);
	character = new btKinematicCharacterController(this->ghost, capsule, stepHeight);

	st->physics->addCollisionObject(this->ghost, CollisionGroup::CG_UNIT);
	st->physics->addAction(character);



	this->body = NULL;
	//this->body->setUserPointer(this);
}

Unit::~Unit()
{
	delete this->anim;
	delete this->uts;
	//st->physics->delRigidBody(this->body);
}


/**
* Sets the unit state. If the unit state type changes, a new state is randomly
* chosen with the specified state type
**/
void Unit::setState(int new_type)
{
	if (this->current_state_type == new_type) return;
	
	if (new_type == UNIT_STATE_STATIC && this->firing) new_type = UNIT_STATE_FIRING;
	
	this->current_state_type = new_type;
	this->current_state = this->uc->getState(new_type);
	this->animation_start = this->st->anim_frame;
	
	delete (this->anim);
	this->anim = new AnimPlay(this->current_state->model);
}


void Unit::beginFiring()
{
	if (this->weapon == NULL) return;
	
	this->firing = true;
	
	this->setState(UNIT_STATE_FIRING);
	
	Sound* snd = this->weapon->wt->getSound(WEAPON_SOUND_BEGIN);
	weapon_sound = this->st->audio->playSound(snd, true, this);
}

void Unit::endFiring()
{
	if (this->weapon == NULL) return;
	
	this->firing = false;
	
	this->setState(UNIT_STATE_STATIC);
	
	this->st->audio->stopSound(this->weapon_sound);

	Sound* snd = this->weapon->wt->getSound(WEAPON_SOUND_END);
	this->st->audio->playSound(snd, false, this);
}

void Unit::emptySound()
{
	if (this->weapon == NULL) return;
	
	this->st->audio->stopSound(this->weapon_sound);

	// TODO: Fix this
	//Sound* snd = this->weapon->wt->getSound(WEAPON_SOUND_EMPTY);
	//this->st->audio->playSound(snd, true, this);
}



/**
* What is directly in front of a unit, upto range meters ahead
**/
Entity * Unit::infront(float range)
{
	btTransform xform = this->ghost->getWorldTransform();
	
	// Begin and end vectors
	btVector3 begin = xform.getOrigin();
	btVector3 end = begin + xform.getBasis() * btVector3(0, 0, range);
	st->addDebugLine(&begin, &end);
	
	// Do the rayTest
	btCollisionWorld::ClosestRayResultCallback cb(begin, end);
	this->st->physics->getWorld()->rayTest(begin, end, cb);
	
	// Check the raytest result
	if (cb.hasHit()) {
		btRigidBody * body = btRigidBody::upcast(cb.m_collisionObject);
		if (body) {
			return static_cast<Entity*>(body->getUserPointer());
		}
	}
	
	return NULL;
}


/**
* Are we on the ground?
**/
bool Unit::onground()
{
	return this->character->canJump();
}


/**
* Raises or lowers the unit a little so it's on the ground
**/
void Unit::putOnGround(btTransform * xform)
{
	return;		// This method is broken
	
	
	// Falling
	btVector3 begin = xform->getOrigin();
	btVector3 end = xform->getOrigin() + xform->getBasis() * btVector3(0.0f, -2.0f, 0.0f);
	st->addDebugLine(&begin, &end);

	{
	btCollisionWorld::ClosestRayResultCallback cb(begin, end);
	this->st->physics->getWorld()->rayTest(begin, end, cb);

	if (cb.hasHit()) {
		xform->setOrigin(cb.m_hitPointWorld + btVector3(0.0f, 1.0f, 0.0f));
		return;
	}
	}

	// Going up
	begin = xform->getOrigin();
	end = xform->getOrigin() + xform->getBasis() * btVector3(0.0f, 2.0f, 0.0f);
	st->addDebugLine(&begin, &end);

	{
	btCollisionWorld::ClosestRayResultCallback cb(begin, end);
	this->st->physics->getWorld()->rayTest(begin, end, cb);

	if (cb.hasHit()) {
		xform->setOrigin(cb.m_hitPointWorld + btVector3(0.0f, 1.0f, 0.0f));
		return;
	}
	}
}


void Unit::meleeAttack()
{
	if (this->melee_time != 0) return;
	if (this->melee_cooldown != 0) return;
	
	this->melee_time = st->game_time + this->uts->melee_delay;
	this->melee_cooldown = this->melee_time + this->uts->melee_cooldown;
	
	this->setState(UNIT_STATE_MELEE);
	
	Entity *e = this->infront(5.0f);	// TODO: settings (melee range)
	if (e == NULL) return;
	
	DEBUG("weap", "Ray hit %p", e);
	
	if (e->klass() == UNIT) {
		((Unit*)e)->takeDamage(this->uts->melee_damage);
	}
}

void Unit::specialAttack()
{
	// TODO: Code this one
	this->st->addHUDMessage(this->slot, "TODO: special attack");
}


/**
* Pick up a weapon
**/
bool Unit::pickupWeapon(WeaponType* wt)
{
	if (this->pickupAmmo(wt)) return true;
	
	UnitWeapon *uw = new UnitWeapon();
	uw->wt = wt;
	uw->magazine = wt->magazine_limit;
	uw->belt = wt->belt_limit;
	uw->next_use = st->game_time;
	uw->reloading = false;
	
	this->avail_weapons.push_back(uw);
	
	this->setWeapon(this->avail_weapons.size() - 1);
	
	return true;
}

/**
* Pick up ammo
**/
bool Unit::pickupAmmo(WeaponType* wt)
{
	for (unsigned int i = 0; i < this->avail_weapons.size(); i++) {
		if (this->avail_weapons[i]->wt == wt) {
			this->avail_weapons[i]->belt += wt->belt_limit;
			this->avail_weapons[i]->belt = min(this->avail_weapons[i]->belt, wt->belt_limit);
			this->avail_weapons[i]->magazine += wt->magazine_limit;
			this->avail_weapons[i]->magazine = min(this->avail_weapons[i]->magazine, wt->magazine_limit);
			return true;
		}
	}
	
	return false;
}

unsigned int Unit::getNumWeapons()
{
	return this->avail_weapons.size();
}

WeaponType * Unit::getWeaponTypeAt(unsigned int id)
{
	return this->avail_weapons[id]->wt;
}

UnitWeapon * Unit::getWeaponAt(unsigned int id)
{
	return this->avail_weapons[id];
}

/**
* Sets the unit weapon
* id is the weapon id of the units personal weapon cache - not the weapontype id
**/
void Unit::setWeapon(int id)
{
	UnitWeapon *uw = this->avail_weapons.at(id);
	
	this->weapon = uw;
	this->firing = false;
	
	curr_weapon_id = id;
}

unsigned int Unit::getCurrentWeaponID()
{
	return this->curr_weapon_id;
}

unsigned int Unit::getPrevWeaponID()
{
	int ret = this->curr_weapon_id - 1;
	if (ret < 0) ret = this->avail_weapons.size() - 1;
	return ret;
}

unsigned int Unit::getNextWeaponID()
{
	unsigned int ret = this->curr_weapon_id + 1;
	if (ret > this->avail_weapons.size() - 1) ret = 0;
	return ret;
}

/**
* The amount of ammo in the belt, or -1 if no current weapon
**/
int Unit::getBelt()
{
	if (this->weapon == NULL) return -1;
	return this->weapon->belt;
}

/**
* The amount of ammo in the magazine, or -1 no current weapon, or -2 if reloading
**/
int Unit::getMagazine()
{
	if (this->weapon == NULL) return -1;
	if (this->weapon->reloading) return -2;
	return this->weapon->magazine;
}

/**
* Gets the unit health
**/
int Unit::getHealth()
{
	return this->health;
}



AnimPlay* Unit::getAnimModel()
{
	return this->anim;
}

Sound* Unit::getSound()
{
	return NULL;
}


/**
* Get the current UnitTypeSettings object.
* It's updated each time the modifiers are changed
**/
UnitTypeSettings * Unit::getSettings()
{
	return this->uts;
}


/**
* Get the Uint8 bitfield of modifiers
**/
Uint8 Unit::getModifiers()
{
	return this->modifiers;
}


/**
* Set all the modifiers
**/
void Unit::setModifiers(Uint8 modifiers)
{
	this->modifiers = modifiers;
	delete this->uts;
	this->uts = this->uc->getSettings(this->modifiers);
}


/**
* Turn on a modifier
**/
void Unit::addModifier(int modifier)
{
	this->modifiers |= modifier;
	delete this->uts;
	this->uts = this->uc->getSettings(this->modifiers);
}


/**
* Turn off a modifier
**/
void Unit::remModifier(int modifier)
{
	this->modifiers &= ~modifier;
	delete this->uts;
	this->uts = this->uc->getSettings(this->modifiers);
}



/**
* Moves units around
**/
void Unit::update(int delta)
{
	if (remove_at != 0) {
		if (remove_at <= st->game_time) this->del = 1;
		return;
	}
	
	
	if (st->server) {
		st->server->addmsgUnitState(this);
	}


	// If they have fallen a considerable distance, they die
	btTransform xform = ghost->getWorldTransform();
	if (xform.getOrigin().y() <= -100.0) {
		this->takeDamage(this->health);
	}

	
	// Which weapon to use?
	WeaponType *w = NULL;
	if (this->firing) {
		if (this->turret_obj) {
			w = this->st->mm->getWeaponType("poopgun");
			
		} else if (this->drive) {
			w = this->st->mm->getWeaponType("rocket_launcher");
			
		} else if (this->weapon && this->weapon->next_use < st->game_time && this->weapon->magazine > 0) {
			w = this->weapon->wt;
			
		}
	}
	
	// Fire!
	if (w) {
		w->doFire(this);
		
		if (w == this->weapon->wt) {
			this->weapon->next_use = st->game_time + this->weapon->wt->fire_delay;
			
			this->weapon->magazine--;
			if (this->weapon->magazine == 0 && this->weapon->belt > 0) {
				int load = this->weapon->wt->magazine_limit;
				if (load > this->weapon->belt) load = this->weapon->belt;
				this->weapon->magazine = load;
				this->weapon->belt -= load;
				this->weapon->next_use += this->weapon->wt->reload_delay;
				this->weapon->reloading = true;
				this->endFiring();
				this->emptySound();
			}
		}
		
		if (! w->continuous) this->endFiring();
	}
	
	// Reset the 'reloading' flag if enough time has passed
	if (this->weapon && this->weapon->next_use < st->game_time) {
		this->weapon->reloading = false;
	}


	// Move the lifted object with the unit
	// Or driving, or turrets
	// TODO: New Physics
	/*if (this->lift_obj) {
		this->lift_obj->x = this->x;
		this->lift_obj->y = this->y;
		this->lift_obj->z = 70;
		this->lift_obj->angle = this->angle;
		
	} else if (this->drive_obj) {
		this->drive_obj->x = pointPlusAngleX(this->x, this->angle, -2);
		this->drive_obj->y = pointPlusAngleY(this->y, this->angle, -2);
		this->drive_obj->z = this->z;
		this->drive_obj->angle = this->angle;
		
	} else if (this->turret_obj) {
		this->x = this->turret_obj->x;
		this->y = this->turret_obj->y;
		this->z = this->turret_obj->z + 5;
		this->turret_obj->angle = this->angle;
	}*/
	
	
	
	{
		      btManifoldArray   manifoldArray;
      btBroadphasePairArray& pairArray = ghost->getOverlappingPairCache()->getOverlappingPairArray();
      int numPairs = pairArray.size();

      for (int i=0;i<numPairs;i++)
      {
         manifoldArray.clear();

         const btBroadphasePair& pair = pairArray[i];
         
         //unless we manually perform collision detection on this pair, the contacts are in the dynamics world paircache:
         btBroadphasePair* collisionPair = st->physics->getWorld()->getPairCache()->findPair(pair.m_pProxy0,pair.m_pProxy1);
         if (!collisionPair)
            continue;

         if (collisionPair->m_algorithm)
            collisionPair->m_algorithm->getAllContactManifolds(manifoldArray);

         for (int j=0;j<manifoldArray.size();j++)
         {
            btPersistentManifold* manifold = manifoldArray[j];
            btScalar directionSign = manifold->getBody0() == ghost ? btScalar(-1.0) : btScalar(1.0);

			btCollisionObject* obA = static_cast<btCollisionObject*>(manifold->getBody0());
			btCollisionObject* obB = static_cast<btCollisionObject*>(manifold->getBody1());

			btCollisionObject* other = obA == ghost ? obB : obA;

			if (other->getBroadphaseHandle()->m_collisionFilterGroup == CollisionGroup::CG_DYNAMIC) {
				Object* o = (Object*) other->getUserPointer();

				if (o->ot->over) this->doUse();


			for (int p=0;p<manifold->getNumContacts();p++)
            {
             	const btManifoldPoint&pt = manifold->getContactPoint(p);
                if (pt.getDistance()<0.f)
				{
					const btVector3& ptA = pt.getPositionWorldOnA();
					const btVector3& ptB = pt.getPositionWorldOnB();
					const btVector3& normalOnB = pt.m_normalWorldOnB;
					/// work here
				}
            }

			}


         }
      }
	}


	if (this->anim->isDone()) this->anim->next();
}


/**
* We have been hit! Take some damage
**/
int Unit::takeDamage(int damage)
{
	this->health -= damage;
	
	btTransform xform = this->ghost->getWorldTransform();
	create_particles_blood_spray(this->st, new btVector3(xform.getOrigin()), damage);
	
	this->st->increaseEntropy(1);
	
	if (this->health <= 0 && remove_at == 0) {
		this->setState(UNIT_STATE_DIE);
		this->endFiring();
		this->leaveVehicle();
		//this->st->physics->markDead(this->body);
		
		remove_at = st->game_time + (10 * 60 * 1000);	// 10 mins
		
		return 1;
	}
	
	return 0;
}


/**
* Enter a vehicle
**/
void Unit::enterVehicle(Vehicle *v)
{
	this->drive = v;
	this->render = false;
	//this->body->forceActivationState(DISABLE_SIMULATION);
	
	this->drive->brakeForce = 0.0f;
	this->drive->engineForce = 0.0f;
	this->drive->steering = 0.0f;
}


/**
* Leave a vehicle
**/
void Unit::leaveVehicle()
{
	btTransform trans;
	
	if (! this->drive) return;
	
	trans = this->drive->getTransform();
	trans.setRotation(btQuaternion(0,0,0,1));

	btVector3 spawn = this->st->physics->spawnLocation(trans.getOrigin().x(), trans.getOrigin().z(), 1.0f);
	trans.setOrigin(spawn);
	
	this->ghost->setWorldTransform(trans);
	this->ghost->forceActivationState(ACTIVE_TAG);
	
	this->drive->brakeForce = 10.0f;
	this->drive->engineForce = 0.0f;
	this->drive->steering = 0.0f;
	
	this->render = true;
	this->drive = NULL;
}


/**
* Use an object
**/
void Unit::doUse()
{
	// Leave vehicle
	if (this->drive != NULL) {
		this->leaveVehicle();
		return;
	}

	// Find closest entity
	Entity *closest = this->infront(2.0f);
	if (! closest) return;

	if (closest->klass() == VEHICLE) {
		// If a vehicle, enter it
		this->enterVehicle((Vehicle*)closest);

	} else if (closest->klass() == OBJECT) {
		// If an object, use it
		btTransform trans = this->getTransform();
		ObjectType *ot = ((Object*)closest)->ot;
	
		if (ot->show_message.length() != 0) {
			this->st->addHUDMessage(this->slot, ot->show_message);
		}
	
		if (ot->add_object.length() != 0) {
			Object *nu = new Object(this->st->mm->getObjectType(ot->add_object), this->st, trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ(), trans.getRotation().getZ());
			this->st->addObject(nu);
		}
	
		if (ot->pickup_weapon.length() != 0) {
			WeaponType *wt = this->st->mm->getWeaponType(ot->pickup_weapon);
			if (wt) {
				this->st->addHUDMessage(this->slot, "Picked up a ", wt->title);
				this->pickupWeapon(wt);
				closest->del = 1;
			}
		}
	
		if (ot->ammo_crate.length() != 0) {
			if (ot->ammo_crate.compare("current") == 0) {
				if (this->pickupAmmo(weapon->wt)) {
					this->st->addHUDMessage(this->slot, "Picked up some ammo");
					closest->del = 1;
				}
			
			} else {
				WeaponType *wt = this->st->mm->getWeaponType(ot->ammo_crate);
				if (wt && this->pickupAmmo(wt)) {
					this->st->addHUDMessage(this->slot, "Picked up some ammo");
					closest->del = 1;
				}
			}
		}
	
		if (ot->turret == 1) {
			if (this->turret_obj) {
				this->turret_obj = NULL;
				this->setWeapon(this->curr_weapon_id);
			} else {
				this->turret_obj = (Object*)closest;
			}
		}
	}
}

/**
* Lift an object
**/
void Unit::doLift()
{
	Entity *closest = this->infront(2.0f);
	if (! closest) return;

	if (closest->klass() == OBJECT) {
		this->lift_obj = (Object*) closest;
	}
}

/**
* Drop an object. Is this the same as throw?
**/
void Unit::doDrop()
{
	this->lift_obj = NULL;
}

// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include "unit.h"
#include <algorithm>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include "../audio/audio.h"
#include "../game_engine.h"
#include "../game_state.h"
#include "../fx/newparticle.h"
#include "../mod/mod_manager.h"
#include "../mod/objecttype.h"
#include "../mod/pickuptype.h"
#include "../mod/unittype.h"
#include "../mod/vehicletype.h"
#include "../mod/weapontype.h"
#include "../net/net_server.h"
#include "../physics_bullet.h"
#include "../rage.h"
#include "../render_opengl/animplay.h"
#include "../util/btCRKinematicCharacterController.h"
#include "../util/debug.h"
#include "entity.h"
#include "object.h"
#include "pickup.h"
#include "vehicle.h"

class Sound;
class btTransform;
class btVector3;

using namespace std;

PickupType* Unit::initial_pickup;


Unit::Unit(UnitType *uc, GameState *st, float x, float y, float z, Faction fac) : Entity(st)
{
	this->uc = uc;
	this->params = uc->params;
	this->slot = 0;
	this->fac = fac;

	this->health = uc->begin_health;

	this->weapon = NULL;
	this->firing = false;
	this->melee_cooldown = 0;
	this->weapon_sound = -1;
	this->special_firing = false;
	this->special_time = 0;
	this->special_cooldown = 0;

	this->lift_obj = NULL;
	this->drive = NULL;
	this->force = btVector3(0.0f, 0.0f, 0.0f);

	this->anim = new AnimPlay(this->uc->model);

	glm::vec3 translate(0.0f, UNIT_PHYSICS_HEIGHT/-2.0f, 0.0f);
	this->anim->setCustomTransform(translate);

	st->addAnimPlay(this->anim, this);

	// Set animation
	UnitTypeAnimation* uta = this->uc->getAnimation(UNIT_ANIM_STATIC);
	if (uta) {
		this->anim->setAnimation(uta->animation, uta->start_frame, uta->end_frame, uta->loop);
	}

	// Ghost position
	btTransform xform = btTransform(
		btQuaternion(btVector3(0,0,1), 0),
		st->physics->spawnLocation(x, y, UNIT_PHYSICS_HEIGHT)
	);

	// Create ghost
	this->ghost = new btPairCachingGhostObject();
	this->ghost->setWorldTransform(xform);
	this->ghost->setCollisionShape(uc->col_shape);
	this->ghost->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);
	this->ghost->setUserPointer(this);

	// Create Kinematic Character Controller
	btScalar stepHeight = btScalar(0.25);
	this->character = new btCRKinematicCharacterController(this->ghost, uc->col_shape, stepHeight);

	// Add character and ghost to the world
	st->physics->addCollisionObject(this->ghost, CG_UNIT);
	st->physics->addAction(character);

	// Give them some weapons
	vector<WeaponType*>* spawn = st->getSpawnWeapons(this->uc, this->fac);
	for (unsigned int i = 0; i < spawn->size(); i++) {
		this->pickupWeapon(spawn->at(i));
	}
	delete(spawn);

	// Create the initial pickup used for invinciblity
	if (! Unit::initial_pickup) {
		Unit::initial_pickup = new PickupType();
		Unit::initial_pickup->type = PICKUP_TYPE_POWERUP;
		Unit::initial_pickup->perm = new PickupTypeAdjust();
		Unit::initial_pickup->temp = new PickupTypeAdjust();
		Unit::initial_pickup->temp->invincible = true;
	}

	// Make them invincible for a little while
	Unit::initial_pickup->doUse(this);
	UnitPickup up;
	up.pt = Unit::initial_pickup;
	up.u = this;
	up.end_time = st->game_time + 2500;
	pickups.push_back(up);

	this->body = NULL;
}

Unit::~Unit()
{
	st->remAnimPlay(this->anim);
	delete(this->anim);
	this->anim = NULL;

	st->physics->delAction(this->character);
	this->character = NULL;

	st->physics->delCollisionObject(this->ghost);
	this->ghost = NULL;
}


/**
* Set the firing flag and start playing sfx
**/
void Unit::beginFiring()
{
	if (this->weapon == NULL) return;

	this->firing = true;

	Sound* snd = this->weapon->wt->getSound(WEAPON_SOUND_BEGIN);
	GEng()->audio->playSound(snd, false, this);

	// TODO: This should only be after the WEAPON_SOUND_BEGIN sound has finished...
	if (this->weapon->wt->continuous) {
		Sound* snd = this->weapon->wt->getSound(WEAPON_SOUND_REPEAT);
		weapon_sound = GEng()->audio->playSound(snd, this->weapon->wt->continuous, this);
	}
}


/**
* Unset the firing flag and stop sfx
**/
void Unit::endFiring()
{
	if (this->weapon == NULL) return;

	this->firing = false;

	if (this->weapon->wt->continuous) {
		GEng()->audio->stopSound(this->weapon_sound);
	}

	Sound* snd = this->weapon->wt->getSound(WEAPON_SOUND_END);
	GEng()->audio->playSound(snd, false, this);
}


/**
* Play the sound of an empty weapon trying to fire
* TODO: Fix this and get it working
**/
void Unit::emptySound()
{
	if (this->weapon == NULL) return;

	GEng()->audio->stopSound(this->weapon_sound);

	// TODO: Fix this
	//Sound* snd = this->weapon->wt->getSound(WEAPON_SOUND_EMPTY);
	//GEng()->audio->playSound(snd, true, this);
}


/**
* What is directly in front of a unit, upto range meters ahead
**/
Entity * Unit::infront(float range)
{
	btTransform xform = this->ghost->getWorldTransform();
	return this->raytest(xform.getBasis(), range);
}


/**
* Do a raytest outwards from the center of a unit in a certain direction
**/
Entity * Unit::raytest(btMatrix3x3 &direction, float range)
{
	btTransform xform = this->ghost->getWorldTransform();

	btVector3 offGround = btVector3(0.0f, 0.4f, 0.0f);

	// Begin and end vectors
	btVector3 begin = xform.getOrigin() + offGround;
	btVector3 end = begin + offGround + direction * btVector3(0.0f, 0.0f, range);
	st->addDebugLine(&begin, &end);

	// Do the rayTest
	btCollisionWorld::ClosestRayResultCallback cb(begin, end);
	cb.m_collisionFilterGroup = CG_UNIT;
	cb.m_collisionFilterMask = PhysicsBullet::mask_entities;
	this->st->physics->getWorld()->rayTest(begin, end, cb);

	// Check the raytest result
	if (cb.hasHit()) {
		return static_cast<Entity*>(cb.m_collisionObject->getUserPointer());
	}

	return NULL;
}


/**
* Are we on the ground?
* TODO: Fix this, the unit could be falling
**/
bool Unit::onground()
{
	return this->character->canJump();
}


/**
* Do a melee attack directly forwards
**/
void Unit::meleeAttack()
{
	btTransform xform = this->ghost->getWorldTransform();
	this->meleeAttack(xform.getBasis());
}


/**
* Do a melee attack in a specific direction
**/
void Unit::meleeAttack(btMatrix3x3 &direction)
{
	DEBUG("weap", "%p meleeAttack; currtime: %i cooldown: %i", this, st->game_time, this->melee_cooldown);

	if (this->melee_cooldown > st->game_time) return;

	this->melee_cooldown = st->game_time + this->params.melee_delay + this->params.melee_cooldown;

	// TODO: should we hack the direction to only allow X degrees of freedom in front of the unit?

	Entity *e = this->raytest(direction, 5.0f);	// TODO: unit settings (melee range)
	if (e == NULL) return;

	DEBUG("weap", "%p meleeAttack; ray hit %p", this, e);

	if (e->klass() == UNIT) {
		(static_cast<Unit*>(e))->takeDamage(this->params.melee_damage);
	}
}


/**
* Begin a special attack, if there isn't one alreayd running or cooling down
* We also set the end time for the attack (->special_time)
**/
void Unit::specialAttack()
{
	if (this->uc->special_weapon == NULL) return;
	if (this->special_time != 0) return;
	if (this->special_cooldown > st->game_time) return;

	this->special_firing = true;
	this->special_time = st->game_time + this->params.special_delay;
}


/**
* End a special attack and determine how long we have to wait (->special_cooldown)
**/
void Unit::endSpecialAttack()
{
	if (this->uc->special_weapon == NULL) return;
	if (this->special_cooldown > st->game_time) return;

	this->special_time = 0;
	this->special_cooldown = st->game_time + this->params.special_cooldown;
	this->special_firing = false;
}


/**
* Pick up a weapon
**/
bool Unit::pickupWeapon(WeaponType* wt)
{
	if (this->pickupAmmo(wt)) return true;

	UnitWeapon *uw = new UnitWeapon(wt, wt->magazine_limit, wt->belt_limit, st->game_time, false);
	this->avail_weapons.push_back(uw);

	if (this->avail_weapons.size() == 1) {
		this->setWeapon(0);
	}

	return true;
}


/**
* Pick up ammo
**/
bool Unit::pickupAmmo(WeaponType* wt)
{
	if (wt == NULL) return false;

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


/**
* Return the unber of weapons currently in posession
**/
unsigned int Unit::getNumWeapons()
{
	return this->avail_weapons.size();
}


/**
* Return the weapon type at a given index
**/
WeaponType * Unit::getWeaponTypeAt(unsigned int id)
{
	return this->avail_weapons[id]->wt;
}


/**
* Return the type of the currently selected weapon
**/
WeaponType * Unit::getWeaponTypeCurr()
{
	if (this->weapon == NULL) return NULL;
	return this->weapon->wt;
}


/**
* Return the weapon details at a given index
**/
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


/**
* Get the ID of the currently selected weapon
**/
unsigned int Unit::getCurrentWeaponID()
{
	return this->curr_weapon_id;
}


/**
* Get the ID of the previous weapon in the list
**/
unsigned int Unit::getPrevWeaponID()
{
	int ret = this->curr_weapon_id - 1;
	if (ret < 0) ret = this->avail_weapons.size() - 1;
	return ret;
}


/**
* Get the ID of the next weapon in the list
**/
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
float Unit::getHealth()
{
	return this->health;
}


/**
* Gets the unit health, as a percent of starting health
**/
float Unit::getHealthPercent()
{
	return static_cast<float>(this->health) / static_cast<float>(this->uc->begin_health);
}


/**
* Get the current sound
* TODO: Remove - not in use...?
**/
Sound* Unit::getSound()
{
	return NULL;
}


/**
* Removes any pickups which are due to finish,
* and runs their finished() method.
**/
bool remove_finished_pickup(const UnitPickup& up)
{
	if (up.end_time > up.u->getGameState()->game_time) return false;

	up.pt->finished(up.u);

	return true;
}


/**
* Return the current transform
**/
btTransform const &Unit::getTransform() const
{
	return ghost->getWorldTransform();
}


/**
* Set the current transform
**/
void Unit::setTransform(btTransform &t) {
	ghost->setWorldTransform(t);
}


/**
* Called once per tick to do all of the logic and stuff
**/
void Unit::update(int delta)
{
	if (GEng()->server != NULL) {
		GEng()->server->addmsgUnitState(this);
	}

	// If in a vehicle, move the ghost to AIs know where the unit is
	if (this->drive) {
		this->ghost->setWorldTransform(this->drive->getTransform());
	}

	// If they have fallen a considerable distance, they die
	btTransform xform = ghost->getWorldTransform();
	if (xform.getOrigin().y() <= -100.0) {
		this->takeDamage(this->health);
	}


	// Which weapon to use?
	WeaponType *w = NULL;
	btTransform wxform;
	if (this->firing) {
		if (this->drive) {
			w = this->drive->vt->weapon_primary;
			wxform = btTransform();
			this->drive->getWeaponTransform(wxform);
		} else if (this->weapon && this->weapon->next_use < st->game_time && this->weapon->magazine > 0) {
			w = this->weapon->wt;
			wxform = btTransform(xform);
		}
	}

	// Fire!
	if (w != NULL) {
		w->doFire(this, wxform);

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

	// Move any object which is being "lifted".
	if (this->lift_obj) {
		btVector3 pos = xform.getOrigin() + xform.getBasis() * btVector3(0.0f, 0.0f, 1.2f);
		btTransform lift(xform.getBasis(), pos);
		this->lift_obj->setTransform(lift);
	}

	// Do the special attack
	// If it's single-shot or run for too long, we stop it
	if (special_firing) {
		w = this->uc->special_weapon;

		w->doFire(this, xform);

		if (!w->continuous || st->game_time > this->special_time) {
			this->endSpecialAttack();
		}
	}

	// Iterate through the physics pairs to see if there are any Pickups to pick up.
	{
		btManifoldArray   manifoldArray;
		btBroadphasePairArray& pairArray = ghost->getOverlappingPairCache()->getOverlappingPairArray();
		int numPairs = pairArray.size();

		for (int i = 0; i < numPairs; i++){
			manifoldArray.clear();

			const btBroadphasePair& pair = pairArray[i];

			//unless we manually perform collision detection on this pair, the contacts are in the dynamics world paircache:
			btBroadphasePair* collisionPair = st->physics->getWorld()->getPairCache()->findPair(pair.m_pProxy0,pair.m_pProxy1);
			if (!collisionPair) continue;

			if (collisionPair->m_algorithm) {
				collisionPair->m_algorithm->getAllContactManifolds(manifoldArray);
			}

			for (int j = 0; j < manifoldArray.size(); j++) {
				btPersistentManifold* manifold = manifoldArray[j];

				const btCollisionObject* obA = static_cast<const btCollisionObject*>(manifold->getBody0());
				const btCollisionObject* obB = static_cast<const btCollisionObject*>(manifold->getBody1());

				const btCollisionObject* other = obA == ghost ? obB : obA;

				if (other->getBroadphaseHandle()->m_collisionFilterGroup == CG_PICKUP) {
					Pickup* p = static_cast<Pickup*>(other->getUserPointer());
					if (p->doUse(this)) {
						UnitPickup up;
						up.pt = p->getPickupType();
						up.u = this;
						up.end_time = st->game_time + up.pt->getDelay();
						pickups.push_back(up);
					}
				}
			}
		}
	}

	// Remove (and rollback) old pickups
	pickups.remove_if(remove_finished_pickup);
}


/**
* We have been hit! Take some damage
* Returns 1 if the unit died, 0 if it did not
**/
int Unit::takeDamage(float damage)
{
	if (this->params.invincible) {
		return 0;
	}

	this->health -= damage;

	btTransform xform = this->ghost->getWorldTransform();
	create_particles_blood_spray(this->st, xform.getOrigin(), damage);

	this->st->increaseEntropy(1);

	if (this->health <= 0 && this->del == false) {
		this->endFiring();
		this->leaveVehicle();

		// Play a death animation
		UnitTypeAnimation* uta = this->uc->getAnimation(UNIT_ANIM_DEATH);
		if (uta) {
			this->anim->setAnimation(uta->animation, uta->start_frame, uta->end_frame, uta->loop);
		}

		// TODO: play death sound

		// Fling some body parts around
		if (!this->uc->death_debris.empty()) {
			this->st->scatterDebris(this, 3, 5.0f, &this->uc->death_debris);
		}

		this->st->deadButNotBuried(this, this->anim);
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
	this->st->remAnimPlay(this->anim);
	this->ghost->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
	this->drive->enter();
}


/**
* Leave a vehicle
**/
void Unit::leaveVehicle()
{
	if (! this->drive) return;

	btTransform trans = this->drive->getTransform();
	btVector3 size = this->drive->vt->model->getBoundingSizeHE();
	size += btVector3(1.5f, 0.0f, 1.5f);

	// Raycast four candidates for spawn
	btVector3 spawnopts[4];
	spawnopts[0] = this->st->physics->spawnLocation(trans.getOrigin().x() - size.x(), trans.getOrigin().z() - size.z(), 1.0f);
	spawnopts[1] = this->st->physics->spawnLocation(trans.getOrigin().x() + size.x(), trans.getOrigin().z() - size.z(), 1.0f);
	spawnopts[2] = this->st->physics->spawnLocation(trans.getOrigin().x() - size.x(), trans.getOrigin().z() + size.z(), 1.0f);
	spawnopts[3] = this->st->physics->spawnLocation(trans.getOrigin().x() + size.x(), trans.getOrigin().z() + size.z(), 1.0f);

	// Find the lowest one, it's probably on the ground
	btVector3 spawn = spawnopts[0];
	for (int i = 1; i < 4; i++) {
		if (spawnopts[i].y() < spawn.y()) {
			spawn = spawnopts[i];
		}
	}

	// Update unit
	this->ghost->setWorldTransform(btTransform(btQuaternion(0,0,0,1), spawn));
	this->ghost->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);
	this->drive->exit();
	this->st->addAnimPlay(this->anim, this);
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
		this->enterVehicle(static_cast<Vehicle*>(closest));

	} else if (closest->klass() == OBJECT) {
		// If an object, use it
		btTransform trans = this->getTransform();
		ObjectType *ot = (static_cast<Object*>(closest))->ot;

		if (ot->show_message.length() != 0) {
			this->st->addHUDMessage(this->slot, ot->show_message);
		}

		if (ot->add_object.length() != 0) {
			Object *nu = new Object(GEng()->mm->getObjectType(ot->add_object), this->st, trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ(), trans.getRotation().getZ());
			this->st->addObject(nu);
		}

		if (ot->pickup_weapon.length() != 0) {
			WeaponType *wt = GEng()->mm->getWeaponType(ot->pickup_weapon);
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
				WeaponType *wt = GEng()->mm->getWeaponType(ot->ammo_crate);
				if (wt && this->pickupAmmo(wt)) {
					this->st->addHUDMessage(this->slot, "Picked up some ammo");
					closest->del = 1;
				}
			}
		}
	}
}


/**
* Lift an object or an other unit
**/
void Unit::doLift()
{
	Entity *closest = this->infront(2.0f);
	if (! closest) return;

	int klass = closest->klass();
	if (klass == OBJECT || klass == UNIT) {
		this->lift_obj = static_cast<Object*>(closest);
	}
}


/**
* Drop an object.
* TODO: add doThrow()
**/
void Unit::doDrop()
{
	if (this->lift_obj) {
		btTransform xform = ghost->getWorldTransform();
		btVector3 pos = xform.getOrigin();
		btTransform drop(xform.getBasis(), pos);
		this->lift_obj->setTransform(drop);
		this->lift_obj = NULL;
	}
}


/**
* Apply a pickup adjustment to a unit
**/
void Unit::applyPickupAdjust(PickupTypeAdjust* adj)
{
	this->health *= adj->health;
	this->takeDamage(0);		// check the player isn't dead.

	this->params.max_speed *= adj->max_speed;
	this->params.melee_damage *= adj->melee_damage;
	this->params.melee_delay *= adj->melee_delay;
	this->params.melee_cooldown *= adj->melee_cooldown;

	if (adj->invincible) {
		this->params.invincible = true;
	}
}


/**
* Rollback a pickup adjustment
**/
void Unit::rollbackPickupAdjust(PickupTypeAdjust* adj)
{
	this->health /= adj->health;
	this->takeDamage(0);		// check the player isn't dead.

	this->params.max_speed /= adj->max_speed;
	this->params.melee_damage /= adj->melee_damage;
	this->params.melee_delay /= adj->melee_delay;
	this->params.melee_cooldown /= adj->melee_cooldown;

	if (adj->invincible) {
		this->params.invincible = false;
	}
}


/**
* Force the unit to move in a given direction
**/
void Unit::applyForce(btVector3 &force)
{
	this->force += force;
}

// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include "unit.h"
#include "unit_weapon.h"
#include "unit_pickup.h"
#include <algorithm>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <sstream>
#include "../../audio/audio.h"
#include "../../game_engine.h"
#include "../../game_state.h"
#include "../../fx/effects_manager.h"
#include "../../mod/mod_manager.h"
#include "../../mod/objecttype.h"
#include "../../mod/pickuptype.h"
#include "../../mod/unittype.h"
#include "../../mod/vehicletype.h"
#include "../../mod/weapontype.h"
#include "../../net/net_server.h"
#include "../../physics/physics_bullet.h"
#include "../../types.h"
#include "../../render_opengl/animplay.h"
#include "../../render_opengl/assimpmodel.h"
#include "../../physics/kinematic_character_controller.h"
#include "../../physics/dynamic_character_controller.h"
#include "../../util/debug.h"
#include "../entity.h"
#include "../object.h"
#include "../pickup.h"
#include "../vehicle.h"

class btTransform;
class btVector3;

using namespace std;

PickupType* Unit::initial_pickup;


/**
* Called when an animation finishes
**/
void unit_spawn_animation_finished(AnimPlay* play, void* data);


/**
* Spawn a unit at max X/Z coordinates
* Y coordinate is calculated
**/
Unit::Unit(UnitType *ut, GameState *st, Faction fac, float x, float z) : Entity(st)
{
	btTransform loc = btTransform(
		btQuaternion(btVector3(0,0,1), 0),
		st->physics->spawnLocation(x, z, UNIT_PHYSICS_HEIGHT)
	);
	
	this->init(ut, st, fac, loc);
}


/**
* Spawn a unit at specific X/Y/Z coordinates
**/
Unit::Unit(UnitType *ut, GameState *st, Faction fac, float x, float y, float z) : Entity(st)
{
	btTransform loc = btTransform(
		btQuaternion(btVector3(0,0,1), 0),
		btVector3(x, y, z)
	);
	
	this->init(ut, st, fac, loc);
}


/**
* Spawn a unit at specific X/Y/Z coordinates
**/
Unit::Unit(UnitType *ut, GameState *st, Faction fac, btTransform & loc) : Entity(st)
{
	this->init(ut, st, fac, loc);
}


/**
* All the init for a unit
**/
void Unit::init(UnitType *ut, GameState *st, Faction fac, btTransform & loc)
{
	this->uc = ut;
	this->params = ut->params;
	this->slot = 0;
	this->fac = fac;

	this->health = ut->begin_health;
	this->last_hit = st->game_time;

	this->weapon = NULL;
	this->firing = false;
	this->melee_cooldown = 0;
	this->weapon_sound = -1;
	this->special_firing = false;
	this->special_time = 0;
	this->special_cooldown = 0;
	this->weapon_zoom_level = 0;
	this->powerup_weapon = NULL;
	this->active = false;

	this->lift_obj = NULL;
	this->drive = NULL;
	this->force = btVector3(0.0f, 0.0f, 0.0f);

	this->resetIdleTime();

	this->anim = new AnimPlay(this->uc->model);

	glm::vec3 translate(0.0f, UNIT_PHYSICS_HEIGHT/-2.0f, 0.0f);
	this->anim->setCustomTransform(translate);

	if (uc->node_head) {
		this->anim->addMoveNode(uc->node_head);
	}

	st->addAnimPlay(this->anim, this);

	// Set animation
	UnitTypeAnimation* uta = this->uc->getAnimation(UNIT_ANIM_SPAWN);
	if (uta) {
		this->anim->setAnimation(uta->animation, uta->start_frame, uta->end_frame);
		this->anim->setEndedCallback(unit_spawn_animation_finished, (void*)this);
	} else {
		this->spawnAnimationFinished();
	}

	this->createKinematicCtlr(loc);

	// Give them some weapons
	vector<WeaponType*>* spawn = st->getSpawnWeapons(this->uc, this->fac);
	for (unsigned int i = 0; i < spawn->size(); i++) {
		this->pickupWeapon(spawn->at(i));
	}
	delete(spawn);

	// Create the initial pickup used for invincibility
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

	this->removeCtlr();
}


/**
* Create and register a kinematic character controller and associated ghost object
**/
void Unit::createKinematicCtlr(btTransform & loc)
{
	btScalar stepHeight = btScalar(0.25);
	
	btPairCachingGhostObject* ghost = new btPairCachingGhostObject();
	
	this->ghost = ghost;
	this->ghost->setWorldTransform(loc);
	this->ghost->setCollisionShape(this->uc->col_shape);
	this->ghost->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);
	this->ghost->setUserPointer(this);

	this->char_ctlr = new btCRKinematicCharacterController(ghost, this->uc->col_shape, stepHeight);

	st->physics->addCollisionObject(this->ghost, CG_UNIT);
	st->physics->addAction(this->char_ctlr);
}


/**
* Create and register a dynamic character controller and associated ghost object
**/
void Unit::createDynamicCtlr(btTransform & loc)
{
	btDefaultMotionState* motionState = new btDefaultMotionState(loc);
	
	btRigidBody* body = st->physics->addRigidBody(
		uc->col_shape, 50.0f, motionState, CG_UNIT
	);
	
	body->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));
	body->setRestitution(1.f);
	body->setRollingFriction(10.f);
	body->setFriction(0.1f);
	
	this->char_ctlr = new btDynamicCharacterController(body);
	this->ghost = body;
	
	st->physics->addAction(this->char_ctlr);
}


/**
* Unregister and remove character controller and ghost object
**/
void Unit::removeCtlr()
{
	if (this->char_ctlr != NULL) {
		st->physics->delAction(this->char_ctlr);
		this->char_ctlr = NULL;
	}

	if (this->ghost != NULL) {
		st->physics->delCollisionObject(this->ghost);
		this->ghost = NULL;
	}
}


/**
* An animation has finished
**/
void unit_spawn_animation_finished(AnimPlay* play, void* data)
{
	static_cast<Unit*>(data)->spawnAnimationFinished();
}


/**
* Spawn animation has finished - begin the walk animation
**/
void Unit::spawnAnimationFinished()
{
	this->anim->setEndedCallback(NULL);

	UnitTypeAnimation* uta = this->uc->getAnimation(UNIT_ANIM_STATIC);
	if (uta) {
		this->anim->setAnimation(uta->animation, uta->start_frame, uta->end_frame, true);
	}

	this->active = true;
}


/**
* Set the firing flag and start playing sfx
**/
void Unit::beginFiring()
{
	if (this->weapon == NULL) return;
	if (this->weapon->reloading) {
		emptySound();
		return;
	}

	this->firing = true;

	AudioPtr snd = this->weapon->wt->getSound(WEAPON_SOUND_BEGIN);
	GEng()->audio->playSound(snd, false, this);

	// TODO: This should only be after the WEAPON_SOUND_BEGIN sound has finished...
	if (this->weapon->wt->continuous) {
		AudioPtr snd = this->weapon->wt->getSound(WEAPON_SOUND_REPEAT);
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

	AudioPtr snd = this->weapon->wt->getSound(WEAPON_SOUND_END);
	GEng()->audio->playSound(snd, false, this);
}


/**
* Play the sound of an empty weapon trying to fire
**/
void Unit::emptySound()
{
	if (this->weapon == NULL) return;

	GEng()->audio->stopSound(this->weapon_sound);

	// TODO: Fix this
	//AudioPtr snd = this->weapon->wt->getSound(WEAPON_SOUND_EMPTY);
	//GEng()->audio->playSound(snd, true, this);
}


/**
* Play a sound while walking
**/
void Unit::walkSound()
{
	// TODO: Implement this
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
	btTransform xform = this->getTransform();

	btVector3 offGround = btVector3(0.0f, 0.3f, 0.0f);

	// Begin and end vectors
	btVector3 begin = xform.getOrigin() + offGround;
	btVector3 end = begin + offGround + direction * btVector3(0.0f, 0.0f, range);

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
	return this->char_ctlr->canJump();
}


/**
* Do a melee attack directly forwards
**/
void Unit::meleeAttack()
{
	btTransform xform = this->getTransform();
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

	Entity *e = this->raytest(direction, this->params.melee_range);
	if (e == NULL) return;

	DEBUG("weap", "%p meleeAttack; ray hit %p", this, e);

	e->takeDamage(this->params.melee_damage);

	this->resetIdleTime();
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

	// TODO: This is for testing only!!
	uw->attach[WPATT_BARREL] = GEng()->mm->getWeaponAttachment("silencer");

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
* Loop through weapon zoom levels and display current zoom level
**/
void Unit::zoomWeapon()
{
	if (this->weapon == NULL) return;

	this->weapon_zoom_level++;
	this->weapon_zoom_level %= this->weapon->wt->zoom_levels.size();

	ostringstream ss;
	ss << this->getWeaponZoom();
	string s(ss.str());
	this->st->addHUDMessage(this->slot, "Weapon zoom: " + s);
}


/**
* Get the current zoom amount in metres
**/
float Unit::getWeaponZoom()
{
	if (this->weapon == NULL) return 0.0f;
	return this->weapon->wt->zoom_levels[this->weapon_zoom_level];
}


/**
* Return the number of weapons currently in possession
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
	this->weapon_zoom_level = 0;

	curr_weapon_id = id;
}


/**
* Attach an attachment to the current weapon
**/
void Unit::weaponAttach(WeaponAttachmentLocation loc, WeaponAttachment* att)
{
	this->weapon->attach[loc] = att;
}


/**
* Detatch an attachment from the current weapon
**/
void Unit::weaponDetatch(WeaponAttachmentLocation loc)
{
	this->weapon->attach[loc] = NULL;
}


/**
* Return attachment at a given location
**/
WeaponAttachment* Unit::getAttachment(WeaponAttachmentLocation loc)
{
	return this->weapon->attach[loc];
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
	if (this->drive) {
		return this->drive->getTransform();
	} else {
		return this->ghost->getWorldTransform();
	}
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
void Unit::update(float delta)
{
	if (GEng()->server != NULL) {
		GEng()->server->addmsgUnitState(this);
	}

	// Units are not active until spawn animation has finished
	if (!this->active) return;

	// If in a vehicle, move the ghost to AIs know where the unit is
	if (this->drive) {
		this->ghost->setWorldTransform(this->drive->getTransform());
	}

	// If they have fallen a considerable distance, they die
	btTransform xform = this->getTransform();
	if (xform.getOrigin().y() <= -100.0) {
		this->die();
	}

	// Regenerate health
	if (this->health < this->uc->begin_health) {
		int time_since_hit = this->st->game_time - this->last_hit;
		if (time_since_hit >= this->params.health_regen_delay) {
			this->health += round(this->params.health_regen_speed / 1000.0f * delta);
			if (this->health > this->uc->begin_health) {
				this->health = this->uc->begin_health;
			}
		}
	}

	// Which weapon to use?
	WeaponType *w = NULL;
	btTransform wxform;
	if (this->firing) {
		if (this->drive) {
			w = this->drive->vt->weapon_primary;
			wxform = btTransform();
			this->drive->getWeaponTransform(wxform);
		} else if (this->powerup_weapon != NULL) {
			w = this->powerup_weapon;
			wxform = btTransform(xform);
		} else if (this->weapon && this->weapon->next_use < st->game_time && this->weapon->magazine > 0) {
			w = this->weapon->wt;
			wxform = btTransform(xform);
		}
	}

	// Fire!
	if (w != NULL) {
		w->doFire(this, wxform, this->params.weapon_damage);

		if (w == this->weapon->wt) {
			this->weapon->next_use = st->game_time + this->weapon->wt->fire_delay;

			this->weapon->magazine--;
			if (this->weapon->magazine == 0 && this->weapon->belt > 0) {
				this->reload();
				this->emptySound();
			}
		}

		if (! w->continuous) this->endFiring();

		this->resetIdleTime();
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

		w->doFire(this, xform, 1.0f);

		if (!w->continuous || st->game_time > this->special_time) {
			this->endSpecialAttack();
		}

		this->resetIdleTime();
	}

	// TODO: Get this working for dynamic controller
	detectCollision();

	// Remove (and rollback) old pickups
	pickups.remove_if(remove_finished_pickup);
	if (pickups.empty()) {
		this->powerup_weapon = NULL;
		this->powerup_message = "";
	}

	// If too much time has passed, play idle sound
	if (this->idle_sound_time < st->game_time && st->getLocalPlayer(this->slot) == NULL) {
		AudioPtr snd = this->uc->getSound(UNIT_SOUND_STATIC);
		if (snd) {
			GEng()->audio->playSound(snd, false, this);
		}
		this->resetIdleTime();
	}
}


/**
* Iterate through the physics pairs to see if there are any Pickups to pick up
**/
void Unit::detectCollision()
{
	btManifoldArray manifoldArray;
	btPairCachingGhostObject* ghost = static_cast<btPairCachingGhostObject*>(this->ghost);
	
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
				p->doUse(this);
			}
		}
	}
}


/*
* Reload weapon.
* All ammo in the magazine will be lost, unless magazine is full or belt empty.
*/
void Unit::reload()
{
	if (this->weapon->belt <= 0) return;
	if (this->weapon->wt->magazine_limit == this->weapon->magazine) return;

	int load = this->weapon->wt->magazine_limit;
	if (load > this->weapon->belt) load = this->weapon->belt;
	this->weapon->magazine = load;
	this->weapon->belt -= load;
	this->weapon->next_use += this->weapon->wt->reload_delay;
	this->weapon->reloading = true;
	this->endFiring();
}


/**
* Reset the idle timer
**/
void Unit::resetIdleTime()
{
	this->idle_sound_time = st->game_time + 15000;
}


/**
* We have been hit! Take some damage
**/
void Unit::takeDamage(float damage)
{
	if (this->params.invincible) {
		return;
	}

	this->health -= damage;
	this->last_hit = this->st->game_time;

	btTransform xform = this->getTransform();
	GEng()->fx->create_particles_blood_spray(this->st, xform.getOrigin(), damage);

	if (this->health <= 0 && this->del == false) {
		this->die();
	}
}


/**
* Most of the death animation, etc
**/
void Unit::dieAnimSound()
{
	this->endFiring();
	this->leaveVehicle();

	// Remove move nodes so they can be animated
	this->anim->removeMoveNode(uc->node_head);

	// Play a death animation
	UnitTypeAnimation* uta = this->uc->getAnimation(UNIT_ANIM_DEATH);
	if (uta) {
		this->anim->setAnimation(uta->animation, uta->start_frame, uta->end_frame, uta->loop);
	}

	// Play death sound
	AudioPtr snd = this->uc->getSound(UNIT_SOUND_DEATH);
	if (snd) {
		GEng()->audio->playSound(snd, false, this);
	}
}


/**
* The unit has died
**/
void Unit::die()
{
	this->dieAnimSound();

	// TODO: Both of these code paths cause this->del = true
	// which means that the death animation will never actually be seen

	if (!this->uc->death_debris.empty()) {
		// Fling some body parts around
		this->st->scatterDebris(this, 3, 5.0f, this->uc->death_debris);
		this->del = true;
	} else {
		// No body parts - just die
		this->st->deadButNotBuried(this, this->anim);
	}
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
* Add a pickup to the 'active' list for this unit
**/
void Unit::addActivePickup(PickupType* pt)
{
	UnitPickup up;
	up.pt = pt;
	up.u = this;
	up.end_time = st->game_time + pt->getDelay();
	this->pickups.push_back(up);

	if (pt->wt != NULL) {
		this->powerup_weapon = pt->wt;
	}
	if (!pt->message.empty()) {
		this->powerup_message = pt->message;
	}
}


/**
* Does the unit currently have the given pickup?
**/
bool Unit::hasActivePickup(PickupType* pt)
{
	for (list<UnitPickup>::iterator it = this->pickups.begin(); it != this->pickups.end(); ++it) {
		if ((*it).pt == pt) return true;
	}
	return false;
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
			Object *nu = new Object(GEng()->mm->getObjectType(ot->add_object), this->st, trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ());
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
	if (closest) {
		this->doDrop();
		this->lift_obj = closest;
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
	this->takeDamage(0.0f);		// check that the player isn't dead.

	this->params.max_speed *= adj->max_speed;
	this->params.melee_damage *= adj->melee_damage;
	this->params.melee_delay *= adj->melee_delay;
	this->params.melee_cooldown *= adj->melee_cooldown;
	this->params.weapon_damage *= adj->weapon_damage;

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
	this->takeDamage(0.0f);		// check the player isn't dead.

	this->params.max_speed /= adj->max_speed;
	this->params.melee_damage /= adj->melee_damage;
	this->params.melee_delay /= adj->melee_delay;
	this->params.melee_cooldown /= adj->melee_cooldown;
	this->params.weapon_damage /= adj->weapon_damage;

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

// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include "player.h"
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include "../game_engine.h"
#include "../game_state.h"
#include "../script/gamelogic.h"
#include "../rage.h"
#include "../render_opengl/animplay.h"
#include "../game_settings.h"
#include "../render/render.h"
#include "../physics/kinematic_character_controller.h"
#include "../physics/physics_bullet.h"
#include "entity.h"
#include "unit.h"
#include "vehicle.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;


#define DEATH_DELAY_MS 2000


/**
* Spawn player using map X/Z coords
**/
Player::Player(UnitType *uc, GameState *st, Faction fac, int slot, float x, float z) : Unit(uc, st, fac, x, z)
{
	this->resetKeyPress(true);

	this->mouse_angle = 0.0f;
	this->vertical_angle = 0.0f;
	this->walking = false;
	this->anim->pause();
	this->slot = slot;
	this->drive_old = NULL;
	this->death_time = 0;
}


/**
* Spawn player using X/Y/Z coords
**/
Player::Player(UnitType *uc, GameState *st, Faction fac, int slot, float x, float y, float z) : Unit(uc, st, fac, x, y, z)
{
	this->resetKeyPress(true);

	this->mouse_angle = 0.0f;
	this->vertical_angle = 0.0f;
	this->walking = false;
	this->anim->pause();
	this->slot = slot;
	this->drive_old = NULL;
}


/**
* Spawn player using specific coordinates
**/
Player::Player(UnitType *uc, GameState *st, Faction fac, int slot, btTransform & loc) : Unit(uc, st, fac, loc)
{
	this->resetKeyPress(true);

	this->mouse_angle = 0.0f;
	this->vertical_angle = 0.0f;
	this->walking = false;
	this->anim->pause();
	this->slot = slot;
	this->drive_old = NULL;
}


Player::~Player()
{
}


/**
* Resets all pressed keys
**/
void Player::resetKeyPress(bool resetHistory /* = false */)
{
	for (int i = 0; i < 16; i++) {
		this->key[i] = 0;
		if (resetHistory) {
			this->lkey[i] = 0;
		}
	}
}


/**
* Sets a key press
**/
void Player::keyPress(Key idx)
{
	this->key[idx] = 1;
}


/**
* Sets a key release
**/
void Player::keyRelease(Key idx)
{
	this->key[idx] = 0;
}


/**
* Packs a bitfield of keys
**/
Uint16 Player::packKeys()
{
	Uint8 k = 0;
	for (int i = 0; i < 16; i++) {
		k |= this->key[i] << i;
	}
	return k;
}


/**
* Set all keys.
* Bitfield should be a Uint16 of flags, with bit 0 => this->key[0], etc.
**/
void Player::setKeys(Uint16 bitfield)
{
	for (int i = 0; i < 16; i++) {
		this->key[i] = (bitfield & (1 << i)) != 0;
	}
}


/**
* Uses ->key and ->lkey to detect changes to the flags, and
* do the appropriate things
**/
void Player::handleKeyChange()
{
	if (this->key[KEY_FIRE] && !this->lkey[KEY_FIRE] && !this->lift_obj) {
		this->beginFiring();
	} else if (!this->key[KEY_FIRE] && this->lkey[KEY_FIRE]) {
		this->endFiring();
	}

	if (this->key[KEY_USE] && !this->lkey[KEY_USE] && !this->lift_obj) {
		this->doUse();
	}

	if (this->key[KEY_LIFT] && !this->lkey[KEY_LIFT]) {
		if (this->lift_obj) {
			this->doDrop();
		} else {
			this->doLift();
		}
	}

	if (this->key[KEY_MELEE] && !this->lkey[KEY_MELEE] && !this->lift_obj) {
		this->meleeAttack();
	}

	if (this->key[KEY_SPECIAL] && !this->lkey[KEY_SPECIAL] && !this->lift_obj) {
		this->specialAttack();
	} else if (!this->key[KEY_SPECIAL] && this->lkey[KEY_SPECIAL]) {
		this->endSpecialAttack();
	}

	if (this->key[KEY_ZOOM] && !this->lkey[KEY_ZOOM]) {
		this->zoomWeapon();
	}

	for (int i = 0; i < 16; i++) this->lkey[i] = this->key[i];
}


/**
* Sets the player angle to point towards the mouse
**/
void Player::angleFromMouse(int x, int y, int delta)
{
	float sensitivity = 0.2f;	// 0.1 = slow, 1.0 = nuts
	float change_dist;

	change_dist = static_cast<float>(x) * sensitivity;
	this->mouse_angle -= change_dist;

	if (this->drive != this->drive_old) {
		// Player has entered or left a vehicle, reset the vertical angle
		this->vertical_angle = 0.0f;
	}

	// Can't tilt as much in a vehicle
	float max_angle;
	if (this->drive) {
		max_angle = 10.0f;
	} else {
		max_angle = 70.0f;
	}

	// Set vertical angle
	sensitivity *= 0.5f;
	change_dist = static_cast<float>(y) * sensitivity;
	this->vertical_angle -= change_dist;
	if (this->vertical_angle > max_angle) this->vertical_angle = max_angle;
	if (this->vertical_angle < -max_angle) this->vertical_angle = -max_angle;

	this->drive_old = this->drive;
}


/**
* Return the view angle, for camera movement
*
* @return float Degrees
**/
float Player::getRenderAngle()
{
	float angle;
	
	if (this->drive != NULL) {
		btTransform trans = this->drive->getTransform();
		angle = RAD_TO_DEG(PhysicsBullet::QuaternionToYaw(trans.getRotation()));
		
		if (this->drive->vt->hasNode(VEHICLE_NODE_HORIZ)) {
			angle += this->mouse_angle;
		}
		
	} else {
		angle = this->mouse_angle;
	}
	
	return angle;
}


/**
* Uses the currently pressed keys to change the player movement
**/
void Player::update(float delta)
{
	if (this->isDying()) {
		if (this->death_time <= this->st->game_time) {
			this->actualDeath();
		}
		return;
	}

	this->handleKeyChange();

	if (this->drive) {
		this->drive->operate(this, delta, this->key, this->mouse_angle, this->vertical_angle);
		this->resetIdleTime();

	} else {
		bool walking = false;

		// Walking around
		btVector3 walkDirection = btVector3(0.0, 0.0, 0.0);
		btTransform xform = this->getTransform();
		btScalar walkSpeed = this->params.max_speed;

		// Mouse rotation
		btQuaternion rot = btQuaternion(btVector3(0.0f, 1.0f, 0.0f), DEG_TO_RAD(this->mouse_angle));
		ghost->getWorldTransform().setBasis(btMatrix3x3(rot));

		// Forward/backward
		btVector3 forwardDir = xform.getBasis() * btVector3(0.0f, 0.0f, walkSpeed);
		if (this->key[KEY_UP]) {
			walkDirection += forwardDir;
			walking = true;
		} else if (this->key[KEY_DOWN]) {
			walkDirection -= forwardDir;
			walking = true;
		}

		// Strafe
		btVector3 strafeDir = xform.getBasis() * btVector3(walkSpeed, 0.0f, 0.0f);
		if (this->key[KEY_LEFT]) {
			walkDirection += strafeDir;
			walking = true;
		} else if (this->key[KEY_RIGHT]) {
			walkDirection -= strafeDir;
			walking = true;
		}

		// Apply any force present on the unit
		walkDirection += this->force;
		this->char_ctlr->setVelocityForTimeInterval(walkDirection, 1.0f);

		// If "walking" state changes, update animation.
		if (walking && !this->walking) {
			this->anim->resume();
		} else if (!walking && this->walking) {
			this->anim->pause();
		}
		this->walking = walking;

		// Reset the idle timer
		if (this->walking) {
			this->walkSound();
			this->resetIdleTime();
		}

		// Angle the head
		if (uc->node_head) {
			glm::mat4 headxform = glm::mat4();
			headxform = glm::rotate(headxform, this->vertical_angle, glm::vec3(-1.0f, 0.0f, 0.0f));
			this->anim->setMoveTransform(uc->node_head, headxform);
		}
	}


	this->force = btVector3(0.0f, 0.0f, 0.0f);

	Unit::update(delta);
}


/**
* View has entered first-person for the local player
**/
void Player::viewModeFirstPerson()
{
	if (this->ghost != NULL) {
		this->ghost->setCollisionFlags(
			this->ghost->getCollisionFlags() | btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT
		);
	}
	
	this->st->remAnimPlay(this->anim);
}


/**
* View has left first-person for the local player
**/
void Player::viewModeThirdPerson()
{
	if (this->ghost != NULL) {
		this->ghost->setCollisionFlags(
			this->ghost->getCollisionFlags() & ~btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT
		);
	}
	
	// Remove first, in case it's already there
	this->st->remAnimPlay(this->anim);
	this->st->addAnimPlay(this->anim, this);
}


/**
* The player has died
**/
void Player::die()
{
	Unit::dieAnimSound();
	viewModeThirdPerson();
	this->death_time = this->st->game_time + DEATH_DELAY_MS;
}


/**
* Is the player in their death delay?
**/
bool Player::isDying()
{
	return (this->death_time != 0);
}


/**
* How far through the death delay is the player?
* Return value is between 0.0 (just started dying) and 1.0 (almost dead)
**/
float Player::dyingPercent()
{
	if (this->death_time < this->st->game_time) {
		return 1.0;
	}
	
	float delay_remaining = this->death_time - this->st->game_time;
	float percent = (DEATH_DELAY_MS - delay_remaining) / DEATH_DELAY_MS;
	
	return percent;
}


/**
* Okay now the player is actually dead.
**/
void Player::actualDeath()
{
	this->st->logic->raise_playerdied(this->slot);

	this->st->deadButNotBuried(this, this->anim);

	for (unsigned int i = 0; i < this->st->num_local; i++) {
		if (this == this->st->local_players[i]->p) {
			this->st->local_players[i]->p = NULL;
			break;
		}
	}
}

// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <math.h>
#include "../rage.h"
#include "../physics_bullet.h"
#include "../game_state.h"
#include "../render_opengl/animplay.h"
#include "../mod/vehicletype.h"
#include "../game_engine.h"
#include "../net/net_server.h"
#include "vehicle.h"
#include "../util/util.h"
#include "../util/convert.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;


static float	wheelRadius = 0.5f;
static float	wheelWidth = 0.4f;
static float	wheelFriction = 1000;
static float	suspensionStiffness = 20.f;
static float	suspensionDamping = 2.3f;
static float	suspensionCompression = 4.4f;
static float	rollInfluence = 0.1f;
static btVector3 wheelDirectionCS0(0,-1,0);
static btVector3 wheelAxleCS(-1,0,0);



Vehicle::Vehicle(GameState *st) : Entity(st)
{
	this->vehicle_raycaster = NULL;
	this->vehicle = NULL;
	this->anim = NULL;
}

Vehicle::Vehicle(VehicleType *vt, GameState *st, float mapx, float mapy) : Entity(st)
{
	btVector3 sizeHE = vt->model->getBoundingSizeHE();
	
	btTransform trans = btTransform(
		btQuaternion(btScalar(0), btScalar(0), btScalar(0)),
		st->physics->spawnLocation(mapx, mapy, sizeHE.z() * 2.0f)
	);
	
	this->init(vt, st, trans);
}

Vehicle::Vehicle(VehicleType *vt, GameState *st, btTransform &loc) : Entity(st)
{
	this->init(vt, st, loc);
}

void Vehicle::init(VehicleType *vt, GameState *st, btTransform &loc)
{
	this->vt = vt;
	this->anim = new AnimPlay(vt->model);
	this->health = vt->health;

	vector <VehicleTypeNode>::iterator it;
	for (it = this->vt->nodes.begin(); it != this->vt->nodes.end(); it++) {
		this->anim->addMoveNode((*it).node);
	}
	
	this->engineForce = 0.0f;
	this->brakeForce = 0.0f;
	this->steering = 0.0f;

	btVector3 sizeHE = vt->model->getBoundingSizeHE();
	
	// Create rigidbody
	btDefaultMotionState* motionState = new btDefaultMotionState(loc);
	this->body = st->physics->addRigidBody(vt->col_shape, vt->mass, motionState, CG_VEHICLE);
	this->body->setUserPointer(this);
	this->body->setActivationState(DISABLE_DEACTIVATION);
	
	// Create Vehicle
	this->vehicle_raycaster = new ChaoticRageVehicleRaycaster(st->physics->getWorld(), this);
	this->vehicle = new btRaycastVehicle(this->tuning, this->body, this->vehicle_raycaster);
	this->vehicle->setCoordinateSystem(0, 1, 2);
	st->physics->addVehicle(this->vehicle);
	
	// Create and attach wheels
	// TODO: Optimize this for fixed turrets
	{
		btScalar suspensionRestLength(sizeHE.y() + 0.1f);

		this->wheel_shape = new btCylinderShapeX(btVector3(wheelWidth,wheelRadius,wheelRadius));
		
		btVector3 connectionPointCS0;
		float connectionHeight = 0.2f;
		
		
		connectionPointCS0 = btVector3(sizeHE.x(), connectionHeight, sizeHE.y());
		this->vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, this->tuning, false);
		
		connectionPointCS0 = btVector3(-sizeHE.x(), connectionHeight, sizeHE.y());
		this->vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, this->tuning, false);
		
		connectionPointCS0 = btVector3(sizeHE.x(), connectionHeight, -sizeHE.y());
		this->vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, this->tuning, false);
		
		connectionPointCS0 = btVector3(-sizeHE.x(), connectionHeight, -sizeHE.y());
		this->vehicle->addWheel(connectionPointCS0, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, this->tuning, false);
	}
	
	// Set some wheel dynamics
	for (int i = 0; i < this->vehicle->getNumWheels(); i++) {
		btWheelInfo& wheel = this->vehicle->getWheelInfo(i);
		
		wheel.m_suspensionStiffness = suspensionStiffness;
		wheel.m_wheelsDampingRelaxation = suspensionDamping;
		wheel.m_wheelsDampingCompression = suspensionCompression;
		wheel.m_frictionSlip = wheelFriction;
		wheel.m_rollInfluence = rollInfluence;
	}
}

Vehicle::~Vehicle()
{
	delete this->vehicle_raycaster;
	delete this->vehicle;
	delete this->anim;
	
	st->physics->delRigidBody(this->body);
}


/**
* Attach a train vehicle to the next one in the chain
**/
void Vehicle::trainAttachToNext(Vehicle *next)
{
	btVector3 front, back;

	front.setX(this->vt->joint_back.x);
	front.setY(this->vt->joint_back.y);
	front.setZ(this->vt->joint_back.z);

	back.setX(next->vt->joint_front.x);
	back.setY(next->vt->joint_front.y);
	back.setZ(next->vt->joint_front.z);
	
	btPoint2PointConstraint* cons = new btPoint2PointConstraint(*this->body, *next->body, front, back);
	st->physics->getWorld()->addConstraint(cons, true);
	
	cons->setDbgDrawSize(btScalar(5.f));
}


/**
* Do stuff
**/
void Vehicle::update(int delta)
{
	int wheelIndex;

	if (this->health == 0) return;
	
	// Front left
	wheelIndex = 0;
	this->vehicle->setSteeringValue(this->steering,wheelIndex);
	this->vehicle->setBrake(this->brakeForce,wheelIndex);
	
	// Front right
	wheelIndex = 1;
	this->vehicle->setSteeringValue(this->steering,wheelIndex);
	this->vehicle->setBrake(this->brakeForce,wheelIndex);
	
	// Rear left
	wheelIndex = 2;
	this->vehicle->applyEngineForce(this->engineForce,wheelIndex);
	this->vehicle->setBrake(this->brakeForce,wheelIndex);
	
	// Rear right
	wheelIndex = 3;
	this->vehicle->applyEngineForce(this->engineForce,wheelIndex);
	this->vehicle->setBrake(this->brakeForce,wheelIndex);
	
	// Update wheels
	for (int i = 0; i < this->vehicle->getNumWheels(); i++) {
		this->vehicle->updateWheelTransform(i, true);
		
		btTransform newTransform = this->vehicle->getWheelInfo(i).m_worldTransform;
		newTransform.setOrigin(newTransform.getOrigin() - this->vehicle->getChassisWorldTransform().getOrigin());
		
		btScalar m[16];
		newTransform.getOpenGLMatrix(m);
		this->setNodeTransformRelative(VEHICLE_NODE_WHEEL0 + i, glm::make_mat4(m));
	}

	// Send state over network
	// TODO: only when moving
	if (GEng()->server != NULL) {
		GEng()->server->addmsgVehicleState(this);
	}
}


void Vehicle::enter()
{
	this->engineForce = 0.0f;
	this->brakeForce = 0.0f;
	this->steering = 0.0f;
}


void Vehicle::exit()
{
	this->engineForce = 0.0f;
	this->brakeForce = 0.0f;
	this->steering = 0.0f;
}


/**
* Called by the unit to update driving status
**/
void Vehicle::operate(Unit* u, int delta, int key_up, int key_down, int key_left, int key_right, float horiz_angle, float vert_angle)
{
	// Accel and brake
	if (this->vt->engine) {
		if (key_up) {
			this->engineForce = MIN(this->engineForce + this->vt->engine_accel, this->vt->engine_max);
			this->brakeForce = 0.0f;
		} else if (key_down) {
			if (this->vehicle->getCurrentSpeedKmHour() > 0.0) {
				this->brakeForce = MIN(this->brakeForce + this->vt->brake_accel, this->vt->brake_max);
				this->engineForce = 0.0f;
			} else {
				this->engineForce = MAX(this->engineForce - this->vt->reverse_accel, 0.0f - this->vt->reverse_max);
				this->brakeForce = 0.0f;
			}
		} else {
			this->engineForce = MAX(this->engineForce - 20.0f, 0.0f);			// Dampening
			this->brakeForce = MAX(this->brakeForce - 15.0f, 0.0f);
		}
	}
	
	// Steering
	if (this->vt->steer) {
		if (key_left) {
			this->steering = MIN(this->steering + 0.01f, 0.3f);
		} else if (key_right) {
			this->steering = MAX(this->steering - 0.01f, -0.3f);
		} else if (this->steering > 0.0f) {
			this->steering = MAX(this->steering - 0.01f, 0.0f);
		} else if  (this->steering < 0.0f) {
			this->steering = MIN(this->steering + 0.01f, 0.0f);
		}
	}

	// Move nodes
	this->setNodeAngle(VEHICLE_NODE_HORIZ, horiz_angle);
	this->setNodeAngle(VEHICLE_NODE_VERT, vert_angle);
}


AnimPlay* Vehicle::getAnimModel()
{
	return this->anim;
}

Sound* Vehicle::getSound()
{
	return NULL;
}


/**
* Return the weapon transform for this vehicle
**/
void Vehicle::getWeaponTransform(btTransform &xform)
{
	xform = this->getTransform();
	
	VehicleTypeNode *horizNode = this->vt->getNode(VEHICLE_NODE_HORIZ);
	if (horizNode != NULL) {
		glm::mat4 transform = this->anim->getNodeTransform(horizNode->node);

		btVector3 offset = btVector3(transform[3][0], transform[3][1], transform[3][2]);
		xform.setOrigin(xform.getOrigin() + offset);

		transform = glm::rotate(transform, 90.0f, glm::vec3(-1.0f, .0f, 0.0f));

		btMatrix3x3 local;
		glmBullet(transform, local);
		xform.setBasis(xform.getBasis() * local);
	}
}


/**
* We have been hit! Take some damage
**/
void Vehicle::takeDamage(int damage)
{
	this->health -= damage;
	if (this->health < 0) this->health = 0;
	
	for (unsigned int j = 0; j < this->vt->damage_models.size(); j++) {
		VehicleTypeDamage * dam = this->vt->damage_models.at(j);
		
		if (this->health <= dam->health) {
			delete(this->anim);
			this->anim = new AnimPlay(dam->model);
			break;
		}
	}
}


/**
* Handle raycasting for vehicle wheels.
* We detect both terrain and water, but return a "miss" if it hits water.
**/
void* ChaoticRageVehicleRaycaster::castRay(const btVector3& from,const btVector3& to, btVehicleRaycasterResult& result)
{
	btCollisionWorld::ClosestRayResultCallback rayCallback(from,to);
	rayCallback.m_collisionFilterGroup = CG_VEHICLE;
	rayCallback.m_collisionFilterMask = CG_TERRAIN | CG_WATER;

	m_dynamicsWorld->rayTest(from, to, rayCallback);

	if (rayCallback.hasHit())
	{
		if (!this->v->vt->land && rayCallback.m_collisionObject->getBroadphaseHandle()->m_collisionFilterGroup == CG_TERRAIN) {
			return 0;
		}

		if (!this->v->vt->water && rayCallback.m_collisionObject->getBroadphaseHandle()->m_collisionFilterGroup == CG_WATER) {
			return 0;
		}

		const btRigidBody* body = btRigidBody::upcast(rayCallback.m_collisionObject);
        if (body && body->hasContactResponse())
		{
			result.m_hitPointInWorld = rayCallback.m_hitPointWorld;
			result.m_hitNormalInWorld = rayCallback.m_hitNormalWorld;
			result.m_hitNormalInWorld.normalize();
			result.m_distFraction = rayCallback.m_closestHitFraction;
			return (void*)body;
		}
	}

	return 0;
}


/**
* Set the angle for all nodes of a given type
**/
void Vehicle::setNodeAngle(VehicleNodeType type, float angle)
{
	vector <VehicleTypeNode>::iterator it;

	for (it = this->vt->nodes.begin(); it != this->vt->nodes.end(); it++) {
		if ((*it).type == type) {
			glm::mat4 rotation = glm::toMat4(glm::rotate(glm::quat(), angle, (*it).axis));
			this->getAnimModel()->setMoveTransform((*it).node, rotation);
		}
	}
}


/**
* Set the transform for all nodes of a given type. Ignores the 'axis' parameter of the node.
**/
void Vehicle::setNodeTransformRelative(VehicleNodeType type, glm::mat4 transform)
{
	vector <VehicleTypeNode>::iterator it;

	for (it = this->vt->nodes.begin(); it != this->vt->nodes.end(); it++) {
		if ((*it).type == type) {
			this->getAnimModel()->setMoveTransform((*it).node, (*it).node->transform * transform);
		}
	}
}




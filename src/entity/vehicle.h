// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <btBulletDynamicsCommon.h>
#include "entity.h"


class AnimPlay;
class Sound;
class VehicleType;


class Vehicle : public Entity
{
	public:
		virtual EntityClass klass() const { return VEHICLE; }
		
	public:
		VehicleType* vt;
		int health;
		float engineForce;
		float brakeForce;
		float steering;

	protected:
		AnimPlay * anim;
		
	private:
		btRaycastVehicle::btVehicleTuning tuning;
		btVehicleRaycaster* vehicle_raycaster;
		btRaycastVehicle* vehicle;
		btCollisionShape* wheel_shape;
	
	public:
		Vehicle(VehicleType *vt, GameState *st, float mapx, float mapy);
		Vehicle(VehicleType *vt, GameState *st, btTransform & loc);
		void init(VehicleType *vt, GameState *st, btTransform & loc);
		virtual ~Vehicle();
		
	public:
		virtual AnimPlay* getAnimModel();
		virtual Sound* getSound();
		virtual void update(int delta);
		
	public:
		void takeDamage(int damage);
		void operate(Unit* u, int key_up, int key_down, int key_left, int key_right, float horiz_angle, float vertical_angle);
		float getSpeedKmHr();
};


class ChaoticRageVehicleRaycaster : public btVehicleRaycaster
{
	public:
		btDynamicsWorld* m_dynamicsWorld;
		Vehicle* v;

	public:
		ChaoticRageVehicleRaycaster(btDynamicsWorld* world, Vehicle* v)
			: m_dynamicsWorld(world), v(v)
			{}

		virtual void* castRay(const btVector3& from,const btVector3& to, btVehicleRaycasterResult& result);

};

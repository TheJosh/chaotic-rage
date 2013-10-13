// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <btBulletDynamicsCommon.h>
#include "entity.h"
#include "vehicle.h"


class VehicleType;


class Helicopter : public Vehicle
{
	private:
		btRaycastVehicle::btVehicleTuning tuning;
		btVehicleRaycaster* vehicle_raycaster;
		btRaycastVehicle* vehicle;
		btCollisionShape* wheel_shape;
	
	public:
		virtual void init(VehicleType *vt, GameState *st, btTransform & loc);
		virtual void update(int delta);
		virtual void operate(Unit* u, int key_up, int key_down, int key_left, int key_right, float horiz_angle, float vert_angle);
		virtual float getSpeedKmHr();
};
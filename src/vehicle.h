// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


class Vehicle : public Entity
{
	public:
		virtual EntityClass klass() const { return VEHICLE; }
		
	public:
		VehicleType* vt;
		int health;
		
	protected:
		AnimPlay * anim;
		
	private:
		btRaycastVehicle::btVehicleTuning tuning;
		btVehicleRaycaster* vehicle_raycaster;
		btRaycastVehicle* vehicle;
		btCollisionShape* wheel_shape;
	
	public:
		Vehicle(VehicleType *pt, GameState *st, float x, float y, float z, float angle);
		virtual ~Vehicle();
		
	public:
		virtual AnimPlay* getAnimModel();
		virtual Sound* getSound();
		virtual void update(int delta);
		virtual void hasBeenHit(Entity * that);
		
	public:
		void takeDamage(int damage);
};

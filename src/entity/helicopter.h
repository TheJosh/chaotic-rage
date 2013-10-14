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
		bool running;
		float lift;
	
	public:
		Helicopter(VehicleType *vt, GameState *st, float mapx, float mapy);
		Helicopter(VehicleType *vt, GameState *st, btTransform & loc);
		void init(VehicleType *vt, GameState *st, btTransform & loc);
		virtual ~Helicopter();
		
		virtual void update(int delta);
		virtual void enter();
		virtual void operate(Unit* u, int key_up, int key_down, int key_left, int key_right, float horiz_angle, float vert_angle);
		virtual void exit();
};

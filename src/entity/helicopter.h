// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include "vehicle.h"

class GameState;
class Unit;
class VehicleType;
class btTransform;


class Helicopter : public Vehicle
{
	private:
		bool running;
		float lift;
		float forward;
		float yaw;
		float pitch;
		float roll;

	public:
		Helicopter(VehicleType *vt, GameState *st, float x, float z);
		Helicopter(VehicleType *vt, GameState *st, float x, float y, float z);
		Helicopter(VehicleType *vt, GameState *st, btTransform & loc);
		void init(VehicleType *vt, GameState *st, btTransform & loc);
		virtual ~Helicopter();

		virtual void update(int delta);
		virtual void enter();
		virtual void operate(Unit* u, int delta, int key_up, int key_down, int key_left, int key_right, float horiz_angle, float vert_angle);
		virtual void exit();
		virtual void getWeaponTransform(btTransform &xform);
};

#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


class Unit: public Entity
{
	public:
		static const int TURN_SPEED = 20;
		
	public:
		string name;
		
	protected:
		int speed;
		int health;
		int desired_angle;
		
	private:
		int angle;
		
	public:
		Unit();
		~Unit();
		
	public:
		virtual SDL_Surface* getSprite();
		virtual void update(int usdelta);
};

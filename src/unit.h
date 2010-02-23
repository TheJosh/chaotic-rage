#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


class Unit: public Entity
{
	public:
		string name;
		
	protected:
		int speed;
		int health;
		int desired_angle;
		int angle;
		int current_frame;
		
		UnitClass* uc;
		vector<SDL_Surface*>* sprites;
		
	public:
		Unit(UnitClass *unit_class);
		~Unit();
		
	public:
		virtual SDL_Surface* getSprite() = 0;
		virtual void update(int delta) = 0;
	
	protected:
		void update(int delta, UnitClassSettings *ucs);
};

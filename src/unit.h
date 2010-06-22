#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


class Unit: public Entity
{
	public:
		virtual EntityClass klass() const { return UNIT; }
		
	public:
		string name;
		
	protected:
		int speed;
		int health;
		int desired_angle;
		int angle;
		
		WeaponType* weapon;
		ParticleGenerator* weapon_gen;
		bool firing;
		vector<WeaponType*> avail_weapons;
		
		UnitClass* uc;
		UnitClassState* current_state;
		int current_state_type;
		
		unsigned int animation_start;
		vector<SDL_Surface*>* sprites;
		
	public:
		Unit(UnitClass *uc, GameState *st);
		~Unit();
		
	public:
		virtual SDL_Surface* getSprite();
		virtual void update(int delta) = 0;
		
		void setWeapon(WeaponType* wt);
		void beginFiring();
		void endFiring();
		void pickupWeapon(WeaponType* wt);
		unsigned int getNumWeapons();
		WeaponType* getWeaponByID(unsigned int id);
		
	protected:
		void update(int delta, UnitClassSettings *ucs);
		void setState(int new_type);
		
	public:
		virtual int getHeight() { return 32; }		// used for proper z-indexing
};

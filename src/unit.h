// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

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
		int desired_angle_move;
		int angle_move;
		int desired_angle_aim;
		int angle_aim;
		
		WeaponType* weapon;
		ParticleGenerator* weapon_gen;
		bool firing;
		vector<WeaponType*> avail_weapons;
		unsigned int curr_weapon_id;
		
		UnitClass* uc;
		UnitClassState* current_state;
		int current_state_type;
		
		UnitClassState* walk_state;
		
		unsigned int animation_start;
		vector<SpritePtr>* sprites;
		
	public:
		Unit(UnitClass *uc, GameState *st);
		~Unit();
		
	public:
		virtual void getSprite(SpritePtr list [SPRITE_LIST_LEN]);
		virtual void update(int delta) = 0;
		virtual int getWidth() { return 24; };		// todo: read from unitclass
		virtual int getHeight() { return 24; };		// todo: read from unitclass
		
	public:
		virtual void handleEvent(Event * ev);
		
	public:
		void beginFiring();
		void endFiring();
		
		void pickupWeapon(WeaponType* wt);
		unsigned int getNumWeapons();
		WeaponType * getWeaponAt(unsigned int id);
		
		void setWeapon(int id);
		unsigned int getCurrentWeaponID();
		unsigned int getPrevWeaponID();
		unsigned int getNextWeaponID();
		
	protected:
		void update(int delta, UnitClassSettings *ucs);
		void setState(int new_type);
};

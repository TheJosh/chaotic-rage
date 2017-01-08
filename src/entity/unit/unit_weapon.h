// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once


class WeaponType;
class WeaponAttachment;


/**
* The currently available weapons
**/
class UnitWeapon
{
	friend class Unit;


	public:
		UnitWeapon(WeaponType* wt, int magazine, int belt, unsigned int next_use, bool reloading) {
			this->wt = wt;
			this->magazine = magazine;
			this->belt = belt;
			this->next_use = next_use;
			this->reloading = reloading;
			
			for (unsigned int i = 0; i < NUM_WPATT; ++i) {
				this->attach[i] = NULL;
			}
		}
		~UnitWeapon();

	private:
		WeaponType* wt;
		int magazine;
		int belt;
		unsigned int next_use;		// time after which the weapon can be used again
		bool reloading;
		WeaponAttachment* attach[NUM_WPATT];
};

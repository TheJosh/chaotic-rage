#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


class WeaponType
{
	public:
		ParticleGenType *pg;
		
		// dynamic
		int id;
		
	public:
		WeaponType();
};


bool loadAllWeaponTypes();
WeaponType* getWeaponTypeByID(unsigned int id);

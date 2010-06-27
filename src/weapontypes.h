#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


class WeaponType
{
	public:
		ParticleGenType *pg;
		SpritePtr icon_large;
		
		// dynamic
		int id;
		
	public:
		WeaponType();
};


bool loadAllWeaponTypes(Render * render);
WeaponType* getWeaponTypeByID(unsigned int id);
unsigned int getNumWeaponTypes();


// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include "../rage.h"

using namespace std;


// Random numbers
void seedRandom();
int getRandom(int low, int high);
float getRandomf(float low, float high);

// Random 3D bits
int nextPowerOfTwo(int a);

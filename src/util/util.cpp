// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "../rage.h"

using namespace std;



void seedRandom()
{
	srand((unsigned int) time(NULL));
}

int getRandom(int low, int high)
{
	if (high < low) swap(low, high);
	if (high == low) return low;
	return rand() % (high - low + 1) + low;
}

float getRandomf(float low, float high)
{
	if (high < low) swap(low, high);
	if (high == low) return low;
	return ((float)rand()/(float)RAND_MAX) * (high - low) + low;
}


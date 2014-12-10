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

/**
* Gets the next highest power-of-two for a number
**/
int nextPowerOfTwo(int a)
{
	int rval = 1;
	while (rval < a) rval <<= 1;
	return rval;
}


std::string replaceString(std::string subject, const std::string& search, const std::string& replace)
{
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != std::string::npos) {
         subject.replace(pos, search.length(), replace);
         pos += replace.length();
    }
    return subject;
}
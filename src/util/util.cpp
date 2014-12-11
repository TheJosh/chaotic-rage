// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <sstream>
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


/**
* Replace one string with another
**/
std::string replaceString(std::string subject, const std::string& search, const std::string& replace)
{
	size_t pos = 0;
	while ((pos = subject.find(search, pos)) != std::string::npos) {
		 subject.replace(pos, search.length(), replace);
		 pos += replace.length();
	}
	return subject;
}


/**
* Trim a string
**/
std::string trimString(const std::string& str, const std::string& whitespace)
{
	const size_t strBegin = str.find_first_not_of(whitespace);
	if (strBegin == std::string::npos) {
		return "";
	}

	const size_t strEnd = str.find_last_not_of(whitespace);
	const size_t strRange = strEnd - strBegin + 1;

	return str.substr(strBegin, strRange);
}


/**
 * Covert any type of number to a string
 * Source: http://stackoverflow.com/a/13636164
 */
template <typename T>
std::string numberToString(T number)
{
	ostringstream ss;
	ss << number;
	return ss.str();
}

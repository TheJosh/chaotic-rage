// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <string>
#include <iostream>
#include <vector>
#include "../rage.h"

using namespace std;

class GameState;


/**
* Some help for some compilers
**/
#ifdef _WIN32
	#include <windows.h>
	#include <math.h>
	
	#define or ||
	#define and &&

	static inline double round(double val) { return floor(val + 0.5); }
#endif


/**
* Returns the path for a directory which we can put some user data.
* The returned path includes a trailing slash.
*
* Example return value:
*    /home/josh/.chaoticrage/
**/
string getUserDataDir();


/**
* Reports a fatal error, and then exits.
* The message should not include a trailing newline.
**/
void reportFatalError(string msg);


/**
* Returns an array of full paths to user modules
* The returned paths include the ".crk" part.
*
* Example return value:
*    <
*    /home/josh/.chaoticrage/mods/hey.crk
*    /home/josh/.chaoticrage/mods/whoo.crk
*    >
*
* Please free the result when you are done.
**/
vector<string> * getUserModFilenames();


/**
* Loads the mods, in a multi-threaded way, if possible
* True on success, false on failure
**/
bool threadedModLoader(GameState *st);



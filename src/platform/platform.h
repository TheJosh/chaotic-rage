// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <string>
#include <list>
#include <vector>

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

	#define snprintf sprintf_s
#endif


/**
* Switch the cwd to an appropriate directory to find data files
**/
void chdirToDataDir();


/**
* Returns the path for a directory which we can put some user data.
* The returned path includes a trailing slash.
*
* Example return value:
*    /home/josh/.chaoticrage/
**/
std::string getUserDataDir();


/**
* Reports a fatal error, and then exits.
* The message should not include a trailing newline.
**/
void reportFatalError(std::string msg);


/**
* Shows a message box (like a non-fatal error)
**/
void displayMessageBox(std::string msg);


/**
* Returns an array of names of system mods
*
* Example return value:
*    <
*    cr
*    debug
*    >
*
* Please free the result when you are done.
**/
std::list<std::string> * getSystemModNames();


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
std::vector<std::string> * getUserModFilenames();



// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <string>
#include <iostream>
#include <errno.h>
#include "platform.h"
#include <android/log.h>
#include <SDL.h>
using namespace std;

#define APPNAME "ChaoticRage"


/**
* Switch the cwd to an appropriate directory to find data files
**/
void chdirToDataDir()
{
}


/**
* Returns the path for a directory which we can put some user data.
* The returned path includes a trailing slash.
*
* Example return value:
*    /home/josh/.chaoticrage/
**/
string getUserDataDir()
{
	// TODO: Use dedicated folder instead of the root folder of the (emulated)
	// sd-card to store the config files
	return "/sdcard/";
}


/**
* Reports a fatal error, and then exits.
* The message should not include a trailing newline.
**/
void reportFatalError(string msg)
{
	__android_log_print(ANDROID_LOG_ERROR, APPNAME, "Fatal: %s", msg.c_str());
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Fatal", msg.c_str(), NULL);
	exit(1);
}


/**
* Shows a message box (like a non-fatal error)
**/
void displayMessageBox(string msg)
{
	__android_log_print(ANDROID_LOG_WARN, APPNAME, "Warn: %s", msg.c_str());
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Warning", msg.c_str(), NULL);
}


/**
* Returns an array of names of system maps
*
* Example return value:
*    <
*        <therlor_valley, Therlor Valley>
*        <debug, Debug>
*    >
*
* Please free the result when you are done.
**/
std::vector< std::pair<std::string, std::string> > * getSystemMapNames()
{
	std::vector< std::pair<std::string, std::string> > * maps;

	maps = new std::vector< std::pair<std::string, std::string> >();

	maps->push_back(std::pair<std::string,std::string>("therlor_valley", "Therlor Valley"));
	maps->push_back(std::pair<std::string,std::string>("lakeside", "Lakeside"));
	maps->push_back(std::pair<std::string,std::string>("stormy_desert", "Stormy Desert"));
	maps->push_back(std::pair<std::string,std::string>("caves", "Caves"));
	maps->push_back(std::pair<std::string,std::string>("test", "Test"));

	return maps;
}


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
vector<string> * getSystemModNames()
{
	// TODO: Actually code this!
	vector<string> * out = new vector<string>();
	out->push_back("cr");
	out->push_back("australia_day");
	out->push_back("test");
	return out;
}


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
vector<string> * getUserModFilenames()
{
	vector<string> * ret = new vector<string>();
	return ret;
}

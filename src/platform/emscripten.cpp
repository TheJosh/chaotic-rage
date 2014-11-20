// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <string>
#include <iostream>
#include <errno.h>
#include <glob.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <SDL.h>

#include "../game_state.h"

using namespace std;


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
	return "/";
}


/**
* Reports a fatal error, and then exits.
* The message should not include a trailing newline.
**/
void reportFatalError(string msg)
{
	cout << "FATAL ERROR: " << msg << "\n";
	exit(1);
}


/**
* Shows a message box (like a non-fatal error)
**/
void displayMessageBox(string msg)
{
	cout << msg << "\n";
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
	return new vector<string>();
}


/**
* Fake version of the SDL2 key lookup by name
* Only includes letter keys
**/
SDL_Keycode SDL_GetKeyFromName(const char* name)
{
	switch (name[0]) {
		case 'a': return SDLK_a;
		case 'b': return SDLK_b;
		case 'c': return SDLK_c;
		case 'd': return SDLK_d;
		case 'e': return SDLK_e;
		case 'f': return SDLK_f;
		case 'g': return SDLK_g;
		case 'h': return SDLK_h;
		case 'i': return SDLK_i;
		case 'j': return SDLK_j;
		case 'k': return SDLK_k;
		case 'l': return SDLK_l;
		case 'm': return SDLK_m;
		case 'n': return SDLK_n;
		case 'o': return SDLK_o;
		case 'p': return SDLK_p;
		case 'q': return SDLK_q;
		case 'r': return SDLK_r;
		case 's': return SDLK_s;
		case 't': return SDLK_t;
		case 'u': return SDLK_u;
		case 'v': return SDLK_v;
		case 'w': return SDLK_w;
		case 'x': return SDLK_x;
		case 'y': return SDLK_y;
		case 'z': return SDLK_z;
		default:
			return SDLK_UNKNOWN;
	}
}



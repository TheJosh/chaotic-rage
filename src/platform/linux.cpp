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

#include "../gamestate.h"

using namespace std;


/**
* Switch the cwd to an appropriate directory to find data files
**/
void chdirToDataDir()
{
	struct stat sb;
	
	// Look in current directory - nothing to do
	if (stat("data", &sb) == 0) {
		return;
	}
	
	// Look in /usr/share
	if (stat("/usr/share/chaoticrage/data", &sb) == 0) {
		chdir("/usr/share/chaoticrage");
		return;
	}
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
	char * env;
	
	env = getenv ("HOME");
	if (! env) {
		reportFatalError("Environment variable $HOME is not set");
	}
	
	string home(env);
	home.append("/.chaoticrage/");
	
	int status = mkdir(home.c_str(), 0777);
	if (status == -1 && errno != EEXIST) {
		reportFatalError("Unable to create directory " + home);
	}
	
	return home;
}


/**
* Reports a fatal error, and then exits.
* The message should not include a trailing newline.
**/
void reportFatalError(string msg)
{
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Chaotic Rage " VERSION, msg.c_str(), NULL);
	cout << "FATAL ERROR: " << msg << "\n";
	exit(1);
}


/**
* Shows a message box (like a non-fatal error)
**/
void displayMessageBox(string msg)
{
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Chaotic Rage " VERSION, msg.c_str(), NULL);
	cout << msg << "\n";
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
list<string> * getSystemModNames()
{
	// TODO: Actually code this!
	list<string> * out = new list<string>();
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
	
	string pat = getUserDataDir();
	pat.append("mods/*.crk");
	
	glob_t glob_result;
	glob(pat.c_str(), GLOB_TILDE, NULL, &glob_result);
	for (unsigned int i = 0; i < glob_result.gl_pathc; ++i) {
		ret->push_back(string(glob_result.gl_pathv[i]));
	}
	globfree(&glob_result);
	
	return ret;
}



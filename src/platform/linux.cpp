// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <string>
#include <iostream>
#include <errno.h>
#include <glob.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <libgen.h>

#include <SDL.h>
#include "../game_engine.h"
#include "../game_state.h"
#include "../render/render.h"
#include "../rage.h"


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

	// Look in /usr/share/chaoticrage
	if (stat("/usr/share/chaoticrage/data", &sb) == 0) {
		if (chdir("/usr/share/chaoticrage") == 0) {
			return;
		}
	}

	cerr << "Could not find data directory\n";
	exit(0);
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
	const char * env = getenv("HOME");
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
	if (GEng()->render->is3D()) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Chaotic Rage " VERSION, msg.c_str(), NULL);
	}
	cout << "FATAL ERROR: " << msg << "\n";
	exit(1);
}


/**
* Shows a message box (like a non-fatal error)
**/
void displayMessageBox(string msg)
{
	if (GEng()->render->is3D()) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Chaotic Rage " VERSION, msg.c_str(), NULL);
	}
	cout << msg << "\n";
}


/**
* Returns an array of names of files and/or directories in a directory
*
* Please free the result when you are done.
*
* @param bool base Return basenames instead of full paths
* @param int type 0 = all, 1 = directories only, 2 = files only
**/
std::vector<std::string> * getDirectoryList(std::string pattern, bool base, int type)
{
	vector<string> * ret = new vector<string>();

	glob_t glob_result;
	struct stat statbuf;

	glob(pattern.c_str(), GLOB_TILDE, NULL, &glob_result);

	for (unsigned int i = 0; i < glob_result.gl_pathc; ++i) {
		if (lstat(glob_result.gl_pathv[i], &statbuf) == -1) {
			continue;
		}
		
		if (type == 1 && S_ISREG(statbuf.st_mode)) continue;
		if (type == 2 && S_ISDIR(statbuf.st_mode)) continue;
		
		if (base) {
			ret->push_back(string(basename(glob_result.gl_pathv[i])));
		} else {
			ret->push_back(string(glob_result.gl_pathv[i]));
		}
	}
	globfree(&glob_result);

	return ret;
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
* Please delete() the result when you are done.
**/
vector<string> * getSystemModNames()
{
	return getDirectoryList("data/*", true, 1);
}


/**
* Returns an array of names of system maps
*
* Example return value:
*    <
*    therlor_valley
*    debug
*    >
*
* Please free the result when you are done.
**/
std::vector<string>* getSystemMapNames()
{
	return getDirectoryList("maps/*", true, 1);
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
* Please delete() the result when you are done.
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

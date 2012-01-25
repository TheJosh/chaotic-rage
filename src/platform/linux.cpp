// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <string>
#include <iostream>
#include "../rage.h"

#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

using namespace std;



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
	cerr << "FATAL ERROR: " << msg << "\n";
	exit(1);
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

	// TODO: Support this method on linux

	return ret;
}
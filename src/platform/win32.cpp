// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <string>
#include <iostream>
#include <shlobj.h>
#include <tchar.h>
#include <shlwapi.h>
#include <SDL.h>
#include <SDL_syswm.h>
#include "platform.h"
#include "../game_state.h"

#pragma comment(lib,"shlwapi.lib")

#define BUFFER_MAX 2048

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
	TCHAR szPath[MAX_PATH];

	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, szPath))) {
		PathAppend(szPath, _T("\\Chaotic Rage\\"));

		CreateDirectory (szPath, NULL);

		return string(szPath);
	}

	return "C:/";
}


/**
* Reports a fatal error, and then exits.
* The message should not include a trailing newline.
**/
void reportFatalError(string msg)
{
	string out = "FATAL ERROR: ";
	out.append(msg);

	MessageBox(HWND_DESKTOP, out.c_str(), "Chaotic Rage", MB_OK);

	exit(1);
}


/**
* Shows a message box (like a non-fatal error)
**/
void displayMessageBox(string msg)
{
	MessageBox(HWND_DESKTOP, msg.c_str(), "Chaotic Rage", MB_OK);
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
	std::vector<string>* maps = new vector<string>();

	WIN32_FIND_DATA fdFile;
	HANDLE hFind = NULL;

	// Initial search
	if ((hFind = FindFirstFile("maps\\*", &fdFile)) == INVALID_HANDLE_VALUE) {
		return maps;
	}

	// Iterate over results
	do {
		if (fdFile.cFileName[0] != '.') {
			if (fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				maps->push_back(string(fdFile.cFileName));
			}
		}
	} while(FindNextFile(hFind, &fdFile));

	FindClose(hFind);

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
	vector<string> * ret = new vector<string>();

	WIN32_FIND_DATA fdFile;
	HANDLE hFind = NULL;

	// Initial search
	if ((hFind = FindFirstFile("data\\*", &fdFile)) == INVALID_HANDLE_VALUE) {
		return ret;
	}

	// Iterate over results
	do {
		if (fdFile.cFileName[0] != '.') {
			if (fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				ret->push_back(string(fdFile.cFileName));
			}
			// TODO: check for .crk's as well
		}
	} while(FindNextFile(hFind, &fdFile));

	FindClose(hFind);

	return ret;
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
	string userdir = getUserDataDir();

	WIN32_FIND_DATA fdFile;
	HANDLE hFind = NULL;
	char sPath[BUFFER_MAX];

	// Initial search
	snprintf(sPath, BUFFER_MAX, "%s\\*.crk", userdir.c_str());
	if ((hFind = FindFirstFile(sPath, &fdFile)) == INVALID_HANDLE_VALUE) {
		return ret;
	}

	// Iterate over results
	do {
		if (fdFile.cFileName[0] != '.') {
			snprintf(sPath, BUFFER_MAX, "%s\\%s", userdir.c_str(), fdFile.cFileName);
			ret->push_back(string(sPath));
		}
	} while(FindNextFile(hFind, &fdFile));

	FindClose(hFind);

	return ret;
}


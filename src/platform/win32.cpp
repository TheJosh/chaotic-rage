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
#include "../rage.h"

#pragma comment(lib,"shlwapi.lib")

using namespace std;


static int threadedModLoader_thread(void *indata);

class ThreadData {
	public:
		GameState * st;
		HDC hdc;
		HGLRC hglrc;
		bool done;
};


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
	list<string> * ret = new list<string>();
	
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
	char sPath[2048];
	
	// Initial search
	sprintf(sPath, "%s\\*.crk", userdir);
	if ((hFind = FindFirstFile(sPath, &fdFile)) == INVALID_HANDLE_VALUE) {
		return ret;
	}
	
	// Iterate over results
	do {
		if (fdFile.cFileName[0] != '.') {
			sprintf(sPath, "%s\\%s", userdir, fdFile.cFileName);
			ret->push_back(string(sPath));
		}
	} while(FindNextFile(hFind, &fdFile));
	
	FindClose(hFind);
	
	return ret;
}


/**
* Loads the mods, in a multi-threaded way, if possible
**/
bool threadedModLoader(GameState *st)
{
	loadMods(st);
	return true;

	SDL_SysWMinfo info;
	SDL_VERSION(&info.version);
	if (! SDL_GetWMInfo(&info)) {
		return false;
	}

	// Create and link contexts
	HDC hdc = GetDC(info.window);
	HGLRC mainContext = wglGetCurrentContext();
	HGLRC loaderContext = wglCreateContext(hdc);
	wglShareLists(mainContext, loaderContext);
	wglMakeCurrent(hdc, mainContext);
	
	// Prep data going to the thread
	ThreadData * td = new ThreadData();
	td->st = st;
	td->hdc = hdc;
	td->hglrc = loaderContext;
	td->done = false;

	// Create and run the thread
	SDL_Thread * thread = SDL_CreateThread(threadedModLoader_thread, td);

	// Wait for the load to finish
	SDL_Event event;
	while (! td->done) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				 SDL_KillThread(thread);
				 return false;
			}
		}
		SDL_GL_SwapBuffers();
		SDL_Delay(50);
	}

	// Just in case
	SDL_WaitThread(thread, NULL);

	wglMakeCurrent(hdc, mainContext);

	return true;
}


/**
* Thread which loads the mods
**/
int threadedModLoader_thread(void *indata)
{
	ThreadData * td = (ThreadData*) indata;
	wglMakeCurrent(td->hdc, td->hglrc);
	loadMods(td->st);
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(td->hglrc);
	td->done = true;
	return 0;
}



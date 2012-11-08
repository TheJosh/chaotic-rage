// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <string>
#include <iostream>
#include <shlobj.h>
#include <tchar.h>
#include <shlwapi.h>
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
void threadedModLoader(GameState *st)
{
	SDL_SysWMinfo info;
	SDL_VERSION(&info.version);
	if (! SDL_GetWMInfo(&info)) {
		exit(1);
	}

	// Create and link contexts
	HDC hdc = GetDC(info.window);
	HGLRC mainContext = wglGetCurrentContext();
	HGLRC loaderContext = wglCreateContext(hdc);
	wglShareLists(loaderContext, mainContext);

	// Prep data going to the thread
	ThreadData * td = new ThreadData();
	td->st = st;
	td->hdc = hdc;
	td->hglrc = loaderContext;
	td->done = false;

	// Create and run the thread
	SDL_Thread * thread = SDL_CreateThread(threadedModLoader_thread, td);

	// Run the intro while we wait
	SDL_WaitThread(thread, NULL);

	wglMakeCurrent(hdc, mainContext);
}


/**
* Thread which loads the mods
**/
int threadedModLoader_thread(void *indata)
{
	ThreadData * td = (ThreadData*) indata;
	wglMakeCurrent(td->hdc, td->hglrc);
	loadMods(td->st);
	wglMakeCurrent(nullptr, nullptr);
	wglDeleteContext(td->hglrc);
	td->done = true;
	return 0;
}



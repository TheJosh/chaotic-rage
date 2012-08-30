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

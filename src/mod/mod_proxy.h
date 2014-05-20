// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <SDL.h>
#include "../rage.h"


using namespace std;

class Mod;


/**
* Proxies one mod into the directory of another
**/
class ModProxy : public Mod {
	public:
		Mod* master;
		string directory;

	public:
		virtual char* loadText(string filename);
		virtual Uint8* loadBinary(string resname, Sint64 *len);
		virtual SDL_RWops* loadRWops(string filename);

	public:
		ModProxy(Mod* master, string directory) : Mod(master->st, master->directory), master(master), directory(directory) {}
		virtual ~ModProxy() {}
};

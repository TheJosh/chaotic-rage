// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <string>
#include <SDL.h>
#include "../rage.h"
#include "mod.h"
#include "mod_proxy.h"

using namespace std;


/**
* Loads a text file into a char * pointer.
* Don't forget to free() when you are done.
**/
char* ModProxy::loadText(string resname)
{
	return this->master->loadText(this->directory + resname);
}


/**
* Loads binary data from the mod.
**/
Uint8 * ModProxy::loadBinary(string resname, int * len)
{
	return this->master->loadBinary(this->directory + resname, len);
}


/**
* Loads a resource into a SDL_RWops.
* Don't forget to SDL_RWclose() when you are done.
**/
SDL_RWops * ModProxy::loadRWops(string resname)
{
	return this->master->loadRWops(this->directory + resname);
}




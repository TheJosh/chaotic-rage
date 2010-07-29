// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include <confuse.h>
#include <zzip/zzip.h>
#include "rage.h"

using namespace std;


Mod::Mod(GameState * st, string directory)
{
	this->st = st;
	this->directory = directory;
}


bool Mod::loadAreaTypes()
{
	return loadAllAreaTypes(this);
}

bool Mod::loadUnitClasses()
{
	return loadAllUnitClasses(this);
}

bool Mod::loadParticleTypes()
{
	return loadAllParticleTypes(this);
}

bool Mod::loadWeaponTypes()
{
	return loadAllWeaponTypes(this);
}


/**
* Loads a text file into a char * pointer.
* Don't forget to free() when you are done.
**/
char * Mod::loadText(string resname)
{
	ZZIP_FILE *fp;
	char *buffer;
	
	string filename = directory;
	filename.append(resname);
	
	fp = zzip_open(filename.c_str(), 0);
	if (! fp) {
		fprintf(stderr, "Couldn't load resource '%s'.\n", resname.c_str());
		return NULL;
	}
	
	// Read the contents of the file into a buffer
	zzip_seek (fp, 0, SEEK_END);
	int len = zzip_tell (fp);
	zzip_seek (fp, 0, SEEK_SET);
	
	buffer = (char*) malloc(len + 1);
	if (buffer == NULL) {
		fprintf(stderr, "Couldn't load resource '%s'.\n", resname.c_str());
		return NULL;
	}
	buffer[len] = '\0';
	
	zzip_read(fp, buffer, len);
	zzip_close(fp);
	
	return buffer;
}


/**
* Loads a resource into a SDL_RWops.
* Don't forget to SDL_RWclose() when you are done.
**/
SDL_RWops * Mod::loadRWops(string resname)
{
	SDL_RWops *rw;
	
	string filename = directory;
	filename.append(resname);
	
	rw = SDL_RWFromZZIP(filename.c_str(), "r");
	if (rw == NULL) {
		fprintf(stderr, "Couldn't load resource '%s'.\n", resname.c_str());
		return NULL;
	}
	
	return rw;
}




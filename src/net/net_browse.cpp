// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL_net.h>
#include <vector>
#include "net.h"
#include "net_browse.h"
#include "../util/ui_update.h"

using namespace std;


/**
* Returns a list of local server names
* Don't forget to delete() the result!
**/
vector<string>* getLocalServers(UIUpdate * ui)
{
	vector<string>* out = new vector<string>();
	
	out->push_back("not");
	out->push_back("yet");
	out->push_back("implemented");
	
	return out;
}


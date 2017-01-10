// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include "../rage.h"

using namespace std;


class UIUpdate;


/**
* Get a list of registered game servers
**/
vector<string> * getServerList(UIUpdate * ui);


/**
* Register a game server
**/
bool registerServer(string name, Uint16 port);
bool unRegisterServer();


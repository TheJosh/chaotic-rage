// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include "../rage.h"

using namespace std;


/**
* Get a list of registered game servers
**/
vector<string> * getServerList();


/**
* Register a game server
**/
void registerServer(string name, string ip, string port);

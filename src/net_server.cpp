// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <math.h>
#include "rage.h"
#include "net.h"

using namespace std;


NetServer::NetServer(GameState * st)
{
	this->st = st;
	st->server = this;
}

NetServer::~NetServer()
{
}


/**
* Some time has passed, do stuff
**/
void NetServer::update()
{
	cout << "Doing NetServer stuff.\n";
}


/**
***  One method for each network message
**/

// TODO: Write them






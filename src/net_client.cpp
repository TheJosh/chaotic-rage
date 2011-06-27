// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <math.h>
#include "rage.h"
#include "net.h"

using namespace std;


NetClient::NetClient(GameState * st)
{
	this->st = st;
	st->client = this;
}

NetClient::~NetClient()
{
}


/**
* Some time has passed, do stuff
**/
void NetClient::update()
{
	cout << "Doing NetClient stuff.\n";
}


/**
***  One method for each network message
**/

void NetClient::addmsgInfoReq() {
}

void NetClient::addmsgJoinReq() {
}

void NetClient::addmsgJoinAck() {
}

void NetClient::addmsgChat() {
}

void NetClient::addmsgKeyMouseStatus() {
}

void NetClient::addmsgQuit() {
}





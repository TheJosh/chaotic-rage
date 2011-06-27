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

void NetServer::addmsgInfoResp() {
}

void NetServer::addmsgJoinAcc() {
}

void NetServer::addmsgJoinRej() {
}

void NetServer::addmsgDataCompl() {
}

void NetServer::addmsgChat() {
}

void NetServer::addmsgUnitAdd() {
}

void NetServer::addmsgUnitUpdate() {
}

void NetServer::addmsgUnitRem() {
}

void NetServer::addmsgWallUpdate() {
}

void NetServer::addmsgWallRem() {
}

void NetServer::addmsgPgAdd() {
}

void NetServer::addmsgPgRem() {
}

void NetServer::addmsgPlayerDrop() {
}

void NetServer::addmsgPlayerQuit() {
}
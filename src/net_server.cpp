// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <math.h>
#include <SDL_net.h>
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
	if (this->sock != NULL) SDLNet_UDP_Close(this->sock);
}


/**
* Some time has passed, do stuff
**/
void NetServer::update()
{
	UDPpacket *pkt = SDLNet_AllocPacket(20);
	
	while (SDLNet_UDP_Recv(this->sock, pkt)) {
		cout << st->game_time << "] Got a packet: " << pkt->data << "\n";
	}
	
	//SDLNet_FreePacket(pkt);
}


/**
* Set the port for listening for connections
**/
void NetServer::listen(int port)
{
	SDLNet_ResolveHost(&this->ipaddress, NULL, port);
	this->sock = SDLNet_UDP_Open(port);
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

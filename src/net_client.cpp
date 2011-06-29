// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <math.h>
#include <SDL_net.h>
#include "rage.h"
#include "net.h"

using namespace std;


NetClient::NetClient(GameState * st)
{
	this->st = st;
	st->client = this;
	this->sock = NULL;
}

NetClient::~NetClient()
{
	if (this->sock != NULL) SDLNet_UDP_Close(this->sock);
}


/**
* Some time has passed, do stuff
**/
void NetClient::update()
{
	UDPpacket *pkt = SDLNet_AllocPacket(1000);
	
	pkt->address = this->ipaddress;
	pkt->len = 0;
	
	Uint8* ptr = pkt->data;
	
	for (list<NetMsg>::iterator it = this->messages.begin(); it != this->messages.end(); it++) {
		*ptr = (*it).type;
		ptr++; pkt->len++;
		
		memcpy(ptr, (*it).data, (*it).size);
		ptr += (*it).size; pkt->len += (*it).size;
	}
	
	cout << setw (6) << setfill(' ') << st->game_time << " SEND ";
	dumpPacket(pkt->data, pkt->len);
	
	SDLNet_UDP_Send(this->sock, -1, pkt);
	
	//SDLNet_FreePacket(pkt);
}


/**
* Set the address and port for sending messages
**/
void NetClient::bind(string address, int port)
{
	SDLNet_ResolveHost(&this->ipaddress, address.c_str(), port);
	this->sock = SDLNet_UDP_Open(0);
}


/**
***  One method for each network message
**/

void NetClient::addmsgInfoReq() {
	NetMsg * msg = new NetMsg(3);
	msg->type = INFO_REQ;
	msg->data[0] = 'A';
	messages.push_back(*msg);
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





// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <math.h>
#include <SDL_net.h>
#include "../rage.h"
#include "net.h"

using namespace std;



// Read funcs (client)
MsgClientRecv msg_client_recv [] = {
	NULL,                               //<--  0x00
	
	NULL,                               //<--  0x01  [C] Req for server info
	&NetClient::handleInfoResp,         //<--  0x02  [S] Server info  (map, gametype, etc)
	NULL,                               //<--  0x03  [C] Join request
	&NetClient::handleJoinAcc,          //<--  0x04  [S] Join accepted
	&NetClient::handleJoinRej,          //<--  0x05  [S] Join denied
	NULL,                               //<--  0x06  [C] Client ack of join accept, server to send data
	&NetClient::handleDataCompl,        //<--  0x07  [S] Sending of data is complete
	NULL,                               //<--  0x08  [C] Client -> server w/ chat msg
	&NetClient::handleChat,             //<--  0x09  [S] Server -> all w/ chat msg
	NULL,                               //<--  0x0A  [C] Keyboard (buttons) and mouse (buttons, position) status -> server
	&NetClient::handleUnitAdd,          //<--  0x0B  [S] A unit has been added
	&NetClient::handleUnitUpdate,       //<--  0x0C  [S] Unit params have changed
	&NetClient::handleUnitRem,          //<--  0x0D  [S] Unit has been removed
	&NetClient::handleWallUpdate,       //<--  0x0E  [S] Wall params have changed
	&NetClient::handleWallRem,          //<--  0x0F  [S] Wall has been removed
	NULL,                               //<--  0x10  -
	NULL,                               //<--  0x11  -
	&NetClient::handlePlayerDrop,       //<--  0x12  [S] Player dropped. too laggy -> all clients
	NULL,                               //<--  0x13  [C] Player want's to leave -> server
	&NetClient::handlePlayerQuit,       //<--  0x14  [S] Player has left -> all clients
	
	NULL,                               //<--  0x15
};


// Read funcs (server)
MsgServerRecv msg_server_recv [] = {
	NULL,                               //<--  0x00
	
	&NetServer::handleInfoReq,          //<--  0x01  [C] Req for server info
	NULL,                               //<--  0x02  [S] Server info  (map, gametype, etc)
	&NetServer::handleJoinReq,          //<--  0x03  [C] Join request
	NULL,                               //<--  0x04  [S] Join accepted
	NULL,                               //<--  0x05  [S] Join denied
	&NetServer::handleJoinAck,          //<--  0x06  [C] Client ack of join accept, server to send data
	NULL,                               //<--  0x07  [S] Sending of data is complete
	&NetServer::handleChat,             //<--  0x08  [C] Client -> server w/ chat msg
	NULL,                               //<--  0x09  [S] Server -> all w/ chat msg
	&NetServer::handleKeyMouseStatus,   //<--  0x0A  [C] Keyboard (buttons) and mouse (buttons, position) status -> server
	NULL,                               //<--  0x0B  [S] A unit has been added
	NULL,                               //<--  0x0C  [S] Unit params have changed
	NULL,                               //<--  0x0D  [S] Unit has been removed
	NULL,                               //<--  0x0E  [S] Wall params have changed
	NULL,                               //<--  0x0F  [S] Wall has been removed
	NULL,                               //<--  0x10  -
	NULL,                               //<--  0x11  -
	NULL,                               //<--  0x12  [S] Player dropped. too laggy -> all clients
	&NetServer::handleQuit,             //<--  0x13  [C] Player want's to leave -> server
	NULL,                               //<--  0x14  [S] Player has left -> all clients
	
	NULL,                               //<--  0x15
};



NetMsg::NetMsg (NetMsgType type, unsigned int size)
{
	this->type = type;
	this->size = size;
	
	this->data = NULL;
	if (size > 0) {
		this->data = (Uint8*) malloc(size);
		memset(this->data, 0, size);
	}
	
	this->dest = NULL;
}

NetMsg::~NetMsg()
{
	free(data);
}


void dumpPacket(Uint8* data, int size)
{
	cout << "{ ";
	for (int i = 0; i < size; i++) {
		cout << hex << setw (2) << setfill('0') << ((int)data[i]) << " ";
	}
	cout << dec << "}" << endl;
}


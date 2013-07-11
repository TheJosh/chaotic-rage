// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <math.h>
#include <SDL_net.h>
#include "../rage.h"
#include "net.h"
#include "net_client.h"
#include "net_server.h"

using namespace std;



// Read funcs (client)
MsgClientRecv msg_client_recv [] = {
	NULL,                               //<--  0x00
	
	NULL,                               //<--  0x01
	&NetClient::handleInfoResp,         //<--  0x02
	NULL,                               //<--  0x03
	&NetClient::handleJoinAcc,          //<--  0x04
	&NetClient::handleJoinRej,          //<--  0x05
	NULL,                               //<--  0x06
	&NetClient::handleDataCompl,        //<--  0x07
	NULL,                               //<--  0x08
	&NetClient::handleChat,             //<--  0x09
	&NetClient::handlePlayerDrop,       //<--  0x0A
	NULL,                               //<--  0x0B
	&NetClient::handlePlayerQuit,       //<--  0x0C
	NULL,                               //<--  0x0D
	&NetClient::handleUnitState,        //<--  0x0E
	&NetClient::handleWallState,        //<--  0x0F
	&NetClient::handleObjectState,      //<--  0x10
	&NetClient::handleVehicleState,     //<--  0x11
	&NetClient::handleEntityRem,        //<--  0x12
	
	NULL,                               //<--  0x13
};


// Read funcs (server)
MsgServerRecv msg_server_recv [] = {
	NULL,                               //<--  0x00
	
	&NetServer::handleInfoReq,          //<--  0x01
	NULL,                               //<--  0x02
	&NetServer::handleJoinReq,          //<--  0x03
	NULL,                               //<--  0x04
	NULL,                               //<--  0x05
	&NetServer::handleJoinAck,          //<--  0x06
	&NetServer::handleJoinDone,         //<--  0x07
	&NetServer::handleChat,             //<--  0x08
	NULL,                               //<--  0x09
	NULL,                               //<--  0x0A
	NULL,                               //<--  0x0B
	NULL,                               //<--  0x0C
	&NetServer::handleKeyMouseStatus,   //<--  0x0D
	NULL,                               //<--  0x0E
	NULL,                               //<--  0x0F
	NULL,                               //<--  0x10
	NULL,                               //<--  0x11
	NULL,                               //<--  0x12
	
	&NetServer::handleQuit,             //<--  0x13
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


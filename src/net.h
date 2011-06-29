// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <ios>
#include <iomanip>
#include <SDL.h>
#include "rage.h"


enum NetMsgType {
	INFO_REQ = 1,			// [C] Req for server info
	INFO_RESP = 2,			// [S] Server info  (map, gametype, etc)
	
	JOIN_REQ = 3,			// [C] Join request
	JOIN_OKAY = 4,			// [S] Join accepted
	JOIN_DENY = 5,			// [S] Join denied
	JOIN_ACK = 6,			// [C] Client ack of join accept, server to send data
	JOIN_DONE = 7,			// [S] Sending of data is complete
	
	CHAT_REQ = 8,			// [C] Client -> server w/ chat msg
	CHAT_MSG = 9,			// [S] Server -> all w/ chat msg
	
	CLIENT_STATE = 10,		// [C] Keyboard (buttons) and mouse (buttons, position) status -> server
	
	UNIT_ADD = 11,			// [S] A unit has been added
	UNIT_UPDATE = 12,		// [S] Unit params have changed
	UNIT_REM = 13,			// [S] Unit has been removed
	
	WALL_UPDATE = 14,		// [S] Wall params have changed
	WALL_REM = 15,			// [S] Wall has been removed
	
	PG_ADD = 16,			// [S] Particle Generator has been added
	PG_REM = 17,			// [S] Particle Generator has been removed
	
	PLAYER_DROP = 18,		// [S] Player dropped. too laggy -> all clients
	REQ_QUIT = 19,			// [C] Player want's to leave -> server
	PLAYER_QUIT = 20,		// [S] Player has left -> all clients
};


class NetMsg {
	public:
		Uint8 type;
		char *data;
		int size;
		
	public:
		NetMsg (unsigned int size);
		~NetMsg();
};


void dumpPacket(Uint8* data, int size);



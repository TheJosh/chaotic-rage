// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"


enum NetMsgType {
	INFO_REQ = 1,			// Req for server info
	INFO_RESP = 2,			// Server info  (map, gametype, etc)
	
	JOIN_REQ = 3,			// Join request
	JOIN_OKAY = 4,			// Join accepted
	JOIN_DENY = 5,			// Join denied
	JOIN_ACK = 6,			// Client ack of join accept, server to send data
	JOIN_DONE = 7,			// Sending of data is complete
	
	CHAT_REQ = 8,			// Client -> server w/ chat msg
	CHAT_MSG = 9,			// Server -> all w/ chat msg
	
	CLIENT_STATE = 10,		// Keyboard (buttons) and mouse (buttons, position) status -> server
	
	UNIT_ADD = 11,			// A unit has been added
	UNIT_UPDATE = 12,		// Unit params have changed
	UNIT_REM = 13,			// Unit has been removed
	
	WALL_UPDATE = 14,		// Wall params have changed
	WALL_REM = 15,			// Wall has been removed
	
	PG_ADD = 16,			// Particle Generator has been added
	PG_REM = 17,			// Particle Generator has been removed
	
	PLAYER_DROP = 18,		// Player dropped. too laggy -> all clients
	REQ_QUIT = 19,			// Player want's to leave -> server
	PLAYER_QUIT = 20,		// Player has left -> all clients
};


class NetMsg {
	Uint16 type;
	char *data;
	
	NetMsg () {
		data = NULL;
	}
	
	~NetMsg() {
		free(data);
	}
};




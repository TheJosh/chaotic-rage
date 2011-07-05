// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <ios>
#include <iomanip>
#include <SDL.h>
#include "rage.h"



class NetServerSeqPred;
class NetServerClientInfo;
class NetClientSeqPred;


/**
* Types of network messages
**/
enum NetMsgType {
	NOTHING =       0x00,
	
	INFO_REQ =      0x01,		// [C] Req for server info
	INFO_RESP =     0x02,		// [S] Server info  (map, gametype, etc)
	
	JOIN_REQ =      0x03,		// [C] Join request
	JOIN_OKAY =     0x04,		// [S] Join accepted
	JOIN_DENY =     0x05,		// [S] Join denied
	JOIN_ACK =      0x06,		// [C] Client ack of join accept, server to send data
	JOIN_DONE =     0x07,		// [S] Sending of data is complete
	
	CHAT_REQ =      0x08,		// [C] Client -> server w/ chat msg
	CHAT_MSG =      0x09,		// [S] Server -> all w/ chat msg
	
	CLIENT_STATE =  0x0A,		// [C] Keyboard (buttons) and mouse (buttons, position) status -> server
	
	UNIT_ADD =      0x0B,		// [S] A unit has been added
	UNIT_UPDATE =   0x0C,		// [S] Unit params have changed
	UNIT_REM =      0x0D,		// [S] Unit has been removed
	
	WALL_UPDATE =   0x0E,		// [S] Wall params have changed
	WALL_REM =      0x0F,		// [S] Wall has been removed
	
	PG_ADD =        0x10,		// [S] Particle Generator has been added
	PG_REM =        0x11,		// [S] Particle Generator has been removed
	
	PLAYER_DROP =   0x12,		// [S] Player dropped. too laggy -> all clients
	QUIT_REQ =      0x13,		// [C] Player want's to leave -> server
	PLAYER_QUIT =   0x14,		// [S] Player has left -> all clients
	
	BOTTOM =        0x15,
};


/**
* Handle a msg recv (client).
*
* @param data A pointer to the data of the message
* @param size The maximum number of bytes which can be read
* @return int The number of bytes read
**/
typedef unsigned int (NetClient::*MsgClientRecv)(Uint8 *data, unsigned int size);

/**
* Array of read funcs
**/
extern MsgClientRecv msg_client_recv [];


/**
* Handle a msg recv (server).
*
* @param data A pointer to the data of the message
* @param size The maximum number of bytes which can be read
* @return int The number of bytes read
**/
typedef unsigned int (NetServer::*MsgServerRecv)(NetServerClientInfo *client, Uint8 *data, unsigned int size);

/**
* Array of read funcs
**/
extern MsgServerRecv msg_server_recv [];


/**
* Sequence number
**/
typedef Uint16 SeqNum;


/**
* A single network message, ready for transmission
**/
class NetMsg {
	public:
		Uint8 type;
		Uint8 *data;
		unsigned int size;
		SeqNum seq;
		
	public:
		NetMsg (NetMsgType type, unsigned int size);
		~NetMsg();
};


/**
* Debugging of packets
**/
void dumpPacket(Uint8* data, int size);


/**
* pack() -- store data dictated by the format string in the buffer
*
*   bits |signed   unsigned   float   string
*   -----+----------------------------------
*      8 |   c        C
*     16 |   h        H         f
*     32 |   l        L         d
*     64 |   q        Q         g
*      - |                               s
*
*  (16-bit unsigned length is automatically prepended to strings)
**/
unsigned int pack(unsigned char *buf, const char *format, ...);


/**
* unpack() -- unpack data dictated by the format string into the buffer
*
*   bits |signed   unsigned   float   string
*   -----+----------------------------------
*      8 |   c        C
*     16 |   h        H         f
*     32 |   l        L         d
*     64 |   q        Q         g
*      - |                               s
*
*  (string is extracted based on its stored length, but 's' can be
*  prepended with a max length)
**/
void unpack(unsigned char *buf, const char *format, ...);



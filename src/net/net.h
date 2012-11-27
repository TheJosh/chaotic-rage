// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <ios>
#include <iomanip>
#include <SDL.h>
#include "../rage.h"



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
	
	PLAYER_DROP =   0x0A,		// [S] Player dropped. too laggy -> all clients
	QUIT_REQ =      0x0B,		// [C] Player want's to leave -> server
	PLAYER_QUIT =   0x0C,		// [S] Player has left -> all clients
	
	CLIENT_STATE =  0x0D,		// [C] Keyboard (buttons) and mouse (buttons, position) status -> server
	
	UNIT_STATE =    0x0E,		// [S] Unit dets have changed
	WALL_STATE =    0x0F,		// [S] Wall dets have changed
	OBJECT_STATE =  0x10,		// [S] Object dets have changed
	VEHICLE_STATE = 0x11,		// [S] Vehicle dets have changed
	ENTITY_REM =    0x12,		// [S] An entity has been removed
	
	BOTTOM =        0x13,
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
		Uint8 uniq;
		Uint8 *data;
		unsigned int size;
		SeqNum seq;
		NetServerClientInfo *dest;
		
	public:
		NetMsg (NetMsgType type, unsigned int size);
		~NetMsg();
};


/**
* Used to find messages
**/
class IsTypeUniqPred
{
	public:
		Uint8 type;
		Uint8 uniq;
		
		bool operator() (const NetMsg& value) { return (value.type == this->type && value.uniq == this->uniq); }
		IsTypeUniqPred(Uint8 type, Uint8 uniq) { this->type = type; this->uniq = uniq; }
};


/**
* Used to find messages
**/
class IsTypeUniqSeqPred
{
	public:
		Uint8 type;
		Uint8 uniq;
		SeqNum seq;
		
		bool operator() (const NetMsg& value) { return (value.type == this->type && value.uniq == this->uniq && value.seq == this->seq); }
		IsTypeUniqSeqPred(Uint8 type, Uint8 uniq, SeqNum seq) { this->type = type; this->uniq = uniq; this->seq = seq; }
};


/**
* Debugging of packets
**/
void dumpPacket(Uint8* data, int size);


/*
** pack() -- store data dictated by the format string in the buffer
**
**  c - 8-bit char
**  h - 16-bit int (signed or unsigned)
**  l - 32-bit int (signed or unsigned)
**  f - float, 32-bit
**  s - string (16-bit length is automatically prepended)
*/ 
int32_t pack(unsigned char *buf, char const *format, ...);


/*
** unpack() -- unpack data dictated by the format string into the buffer
*/
void unpack(unsigned char *buf, char const *format, ...);



// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include <SDL_net.h>
#include "rage.h"
#include "net.h"

using namespace std;


class NetServer {
	friend class NetServerSeqPred;
	
	private:
		GameState * st;
		IPaddress ipaddress;
		UDPsocket sock;
		list<NetMsg> messages;
		SeqNum seq;		// TODO: is this big enough?
		list<NetServerClientInfo*> clients;
		
		NetServerSeqPred * seq_pred;
		
	public:
		NetServer(GameState * st);
		~NetServer();
		
	public:
		void update();
		void listen(int port);
		
	public:
		// One method for each outgoing network message the server sends out
		void addmsgInfoResp();
		void addmsgJoinAcc(NetServerClientInfo *client);
		void addmsgJoinRej();
		void addmsgDataCompl();
		void addmsgChat();
		void addmsgUnitAdd(Unit *u);
		void addmsgUnitUpdate(Unit *u);
		void addmsgUnitRem(Unit *u);
		void addmsgWallUpdate();
		void addmsgWallRem();
		void addmsgPgAdd();
		void addmsgPgRem();
		void addmsgPlayerDrop();
		void addmsgPlayerQuit();
		
	public:
		// One method for each incoming network message from the client
		unsigned int handleInfoReq(NetServerClientInfo *client, Uint8 *data, unsigned int size);
		unsigned int handleJoinReq(NetServerClientInfo *client, Uint8 *data, unsigned int size);
		unsigned int handleJoinAck(NetServerClientInfo *client, Uint8 *data, unsigned int size);
		unsigned int handleChat(NetServerClientInfo *client, Uint8 *data, unsigned int size);
		unsigned int handleKeyMouseStatus(NetServerClientInfo *client, Uint8 *data, unsigned int size);
		unsigned int handleQuit(NetServerClientInfo *client, Uint8 *data, unsigned int size);
		
};


/**
* Used to determine messages that should be removed
**/
class NetServerSeqPred
{
	public:
		NetServer *server;
		bool operator() (const NetMsg& value) { return false; } //(this->server->client_seq > value.seq); }		// todo: client seq array
		NetServerSeqPred(NetServer *s) { this->server = s; }
};


/**
* Client info (ip, code) -> (seq, slot)
**/
class NetServerClientInfo
{
	public:
		IPaddress ipaddress;
		Uint16 code;
		
	public:
		SeqNum seq;		// TODO: again...big enough?
		int slot;
		bool inlist;
		
		NetServerClientInfo() { seq = 0; slot = 0; inlist = false; }
};



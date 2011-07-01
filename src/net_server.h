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


class NetServerSeqPred;

class NetServer {
	friend class NetServerSeqPred;
	
	private:
		GameState * st;
		IPaddress ipaddress;
		UDPsocket sock;
		list<NetMsg> messages;
		unsigned int seq;
		
		unsigned int client_seq;	// todo: client seq array
		
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
		void addmsgJoinAcc();
		void addmsgJoinRej();
		void addmsgDataCompl();
		void addmsgChat();
		void addmsgUnitAdd(Unit *u);
		void addmsgUnitUpdate();
		void addmsgUnitRem();
		void addmsgWallUpdate();
		void addmsgWallRem();
		void addmsgPgAdd();
		void addmsgPgRem();
		void addmsgPlayerDrop();
		void addmsgPlayerQuit();
		
	public:
		// One method for each incoming network message from the client
		unsigned int handleInfoReq(Uint8 *data, unsigned int size);
		unsigned int handleJoinReq(Uint8 *data, unsigned int size);
		unsigned int handleJoinAck(Uint8 *data, unsigned int size);
		unsigned int handleChat(Uint8 *data, unsigned int size);
		unsigned int handleKeyMouseStatus(Uint8 *data, unsigned int size);
		unsigned int handleQuit(Uint8 *data, unsigned int size);
		
};

class NetServerSeqPred
{
	public:
		NetServer *server;
		bool operator() (const NetMsg& value) { return (this->server->client_seq > value.seq); }		// todo: client seq array
		NetServerSeqPred(NetServer *s) { this->server = s; }
};




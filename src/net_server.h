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
		// Server messages
		void addmsgInfoResp();
		void addmsgJoinAcc();
		void addmsgJoinRej();
		void addmsgDataCompl();
		void addmsgChat();
		void addmsgUnitAdd();
		void addmsgUnitUpdate();
		void addmsgUnitRem();
		void addmsgWallUpdate();
		void addmsgWallRem();
		void addmsgPgAdd();
		void addmsgPgRem();
		void addmsgPlayerDrop();
		void addmsgPlayerQuit();
		
	public:
		void handleInfoReq();
};

class NetServerSeqPred
{
	public:
		NetServer *server;
		bool operator() (const NetMsg& value) { return (this->server->client_seq > value.seq); }		// todo: client seq array
		NetServerSeqPred(NetServer *s) { this->server = s; }
};




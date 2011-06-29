// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include <SDL_net.h>
#include <list>
#include "rage.h"
#include "net.h"

using namespace std;


class NetClientSeqPred;

class NetClient {
	friend class NetClientSeqPred;
	
	private:
		GameState * st;
		IPaddress ipaddress;
		UDPsocket sock;
		list<NetMsg> messages;
		unsigned int seq;
		
		NetClientSeqPred * seq_pred;
		
	public:
		NetClient(GameState * st);
		~NetClient();
		
	public:
		void update();
		void bind(string address, int port);
		
	public:
		// Client messages
		void addmsgInfoReq();
		void addmsgJoinReq();
		void addmsgJoinAck();
		void addmsgChat();
		void addmsgKeyMouseStatus();
		void addmsgQuit();
};

class NetClientSeqPred
{
	public:
		NetClient *client;
		bool operator() (const NetMsg& value) { return (this->client->seq > value.seq); }
		NetClientSeqPred(NetClient *c) { this->client = c; }
};




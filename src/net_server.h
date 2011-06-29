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
	private:
		GameState * st;
		IPaddress ipaddress;
		UDPsocket sock;
		list<NetMsg> messages;
		
	public:
		NetServer(GameState * st);
		~NetServer();
		
	public:
		void update();
		void listen(int port);
		
	public:
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
};

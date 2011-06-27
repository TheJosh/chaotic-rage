// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"
#include "net.h"

using namespace std;


class NetServer {
	private:
		GameState * st;
		
	public:
		NetServer(GameState * st);
		~NetServer();
		
	public:
		void update();
		
	public:
		/* TODO: Method signatures for adding Messages */
};

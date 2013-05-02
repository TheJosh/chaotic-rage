// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include <list>
#include "../rage.h"

using namespace std;



class CommandLineArgs
{
	private:
		GameState *st;
		int argc;
		char ** argv;
		
	public:
		string mod;
		string campaign;
		
	public:
		CommandLineArgs(GameState *st, int argc, char ** argv)
			: st(st), argc(argc), argv(argv)
			{}
			
		void process();
};

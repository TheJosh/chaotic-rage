// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include "../rage.h"

using namespace std;


class AnimPlay
{
	private:
		AssimpModel * model;
		int start_frame;
		
	public:
		AnimPlay(AssimpModel * model);
		
	public:
		AssimpModel * getModel();
		int getFrame();
		bool isDone();
		void reset();
		void next();
};

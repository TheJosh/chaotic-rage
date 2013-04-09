// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


class Intro : public UIUpdate
{
	private:
		GameState *st;
		Render *render;
		
		Song* sg;
		SpritePtr img1;
		SpritePtr img2;
		SpritePtr img3;
		int start;
		
	public:
		Intro(GameState *st);
		~Intro();
		void load();
		void doit();
		
	public:
		virtual void updateUI();
};


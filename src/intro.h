// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include "rage.h"
#include "render/render.h"

using namespace std;

class GameState;
class Render;
class Song;


class Intro : public UIUpdate
{
	private:
		GameState *st;
		Render *render;
		
		Song* sg;
		SpritePtr img1;
		SpritePtr img2;
		SpritePtr img3;
		SpritePtr text;
		int start;
		int lasttime;

	public:
		Intro(GameState *st);
		~Intro();
		void load();
		void doit();
		
	public:
		virtual void updateUI();
};


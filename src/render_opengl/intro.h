// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include "../types.h"
#include "../render/render.h"
#include "../util/ui_update.h"

using namespace std;

class GameState;
class RenderOpenGL;
class Song;


class Intro : public UIUpdate
{
	private:
		Mod *mod;
		GameState *st;
		RenderOpenGL *render;

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
		void doit();

	public:
		virtual void updateUI();
};

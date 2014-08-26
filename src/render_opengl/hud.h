// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include "../rage.h"
#include "hud_label.h"

using namespace std;


class RenderOpenGL;


class HUDMessage
{
	friend class HUD;

	public:
		HUDMessage(string text = "") {
			this->text = text;
			this->remove_time = 0;
		}

	private:
		string text;
		unsigned int remove_time;
};


class HUD {
	public:
		PlayerState *ps;
		RenderOpenGL *render;

	private:
		bool weapon_menu;
		list<HUDMessage*> msgs;
		list<HUDLabel*> labels;

	public:
		HUD(PlayerState *ps, RenderOpenGL *render);

	public:
		void draw();

		void eventClick();
		void eventUp();
		void eventDown();

		void addMessage(string text);
		void addMessage(string text1, string text2);

		HUDLabel * addLabel(int x, int y, string data, HUDLabel *l = NULL);
};

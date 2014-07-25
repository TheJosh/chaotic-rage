// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <guichan.hpp>
#include <string>
#include <vector>
#include "../rage.h"
#include "dialog.h"

class GameState;

using namespace std;


/**
* Show a dialog with the settings for the client
**/
class DialogClientSettings : public Dialog, public gcn::ActionListener
{
	private:
		GameState* st;
		gcn::Button* button;

		gcn::TextField* gl_msaa;
		gcn::TextField* gl_tex_filter;
		gcn::CheckBox* fullscreen;
		gcn::DropDown* lang;
		vector<string>* langs;

	public:
		explicit DialogClientSettings(GameState* st);
		virtual ~DialogClientSettings();

		virtual gcn::Container * setup();
		virtual void tearDown();

		virtual const DialogName getName() { return CLIENT_SETTINGS; }
		virtual void action(const gcn::ActionEvent& actionEvent);
};

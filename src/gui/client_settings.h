// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <guichan.hpp>
#include "../rage.h"
#include "dialog.h"

using namespace std;


class RenderOpenGLSettings;

/**
* Show mod selection
**/
class DialogClientSettings : public Dialog, public gcn::ActionListener {
	private:
		GameState* st;
		gcn::Button* button;
		
		gcn::TextField* gl_msaa;
		gcn::TextField* gl_tex_filter;
		
	public:
		DialogClientSettings(GameState* st);
		~DialogClientSettings();
		
		virtual gcn::Container * setup();
		virtual string getName() { return "cl-set"; }
		virtual void action(const gcn::ActionEvent& actionEvent);
};



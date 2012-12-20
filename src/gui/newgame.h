// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include <guichan.hpp>
#include "../rage.h"

using namespace std;


/**
* Dialog for starting a new game - called by the menu
**/
class DialogNewGame : public Dialog, public gcn::ActionListener {
	public:
		DialogNewGame(int num_local);
		
	private:
		int num_local;
		gcn::DropDown * map;
		gcn::DropDown * gametype;
		gcn::DropDown * unittype;
		gcn::DropDown * viewmode;
		
	public:
		virtual gcn::Container * setup();
		virtual void action(const gcn::ActionEvent& actionEvent);
};

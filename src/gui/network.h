// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include <guichan.hpp>
#include "../rage.h"
#include "../http/serverlist.h"

using namespace std;



/**
* Dialog for starting a new game - called by the menu
**/
class DialogNetJoin : public Dialog, public gcn::ActionListener {
	public:
		DialogNetJoin();
		
	private:
		gcn::TabbedArea * tabs;
		gcn::TextField * host;
		gcn::ListBox * internet;
		vector<string> * internet_hosts;
		
	public:
		virtual gcn::Container * setup();
		virtual void action(const gcn::ActionEvent& actionEvent);
};



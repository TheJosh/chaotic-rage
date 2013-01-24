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
* Show mod selection
**/
class DialogMods : public Dialog, public gcn::ActionListener {
	private:
		ModListModel* mods;
		gcn::ListBox* modlist;
		GameState* st;
		ModManager* mm;

	public:
		DialogMods(GameState* st, ModManager* mm);
		~DialogMods();
		
		virtual gcn::Container * setup();
		virtual string getName() { return "mods"; }
		virtual void action(const gcn::ActionEvent& actionEvent);
};



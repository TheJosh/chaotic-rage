// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <guichan.hpp>

using namespace std;


class ModListModel;
class GameState;
class ModManager;


/**
* Show mod selection
**/
class DialogMods : public Dialog, public gcn::ActionListener, public gcn::SelectionListener {
	private:
		ModListModel* mods;
		GameState* st;
		gcn::DropDown* modlist;
		gcn::Icon* icon;
		gcn::Image* img;
		gcn::Button* button;

	public:
		explicit DialogMods(GameState* st);
		virtual ~DialogMods();

		virtual gcn::Container * setup();
		virtual const DialogName getName() { return MODS; }
		virtual void action(const gcn::ActionEvent& actionEvent);
		virtual void valueChanged(const gcn::SelectionEvent& selectionEvent);
};

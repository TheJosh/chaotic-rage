// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <guichan.hpp>
#include "../rage.h"

using namespace std;


class DialogNewGame;


/**
* Sub-dialog from the new game dialog for mucking about with weapons
**/
class DialogNewGameWeapons : public Dialog, public gcn::ActionListener, public gcn::SelectionListener {
	public:
		DialogNewGameWeapons(DialogNewGame* parent, GameSettings* gs, GameType* gt);
		~DialogNewGameWeapons();
		
	protected:
		DialogNewGame* parent;
		GameSettings* gs;
		
		GametypeFactionsListModel* factions_list;
		int faction;
		
		gcn::DropDown* dd_faction;
		gcn::CheckBox* chk_unit;
		gcn::CheckBox* chk_gametype;
		
	public:
		virtual gcn::Container * setup();
		virtual void action(const gcn::ActionEvent& actionEvent);
		virtual void valueChanged(const gcn::SelectionEvent& selectionEvent);
};


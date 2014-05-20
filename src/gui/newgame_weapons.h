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
		virtual ~DialogNewGameWeapons();

	protected:
		DialogNewGame* parent;
		GameSettings* gs;

		GametypeFactionsListModel* factions_list;
		int faction;

		vector<WeaponType*> *wts;

		gcn::DropDown* dd_faction;
		gcn::CheckBox* chk_unit;
		gcn::CheckBox* chk_gametype;
		vector<gcn::CheckBox*> chk_custom;

	public:
		virtual gcn::Container * setup();
		virtual void action(const gcn::ActionEvent& actionEvent);
		virtual void valueChanged(const gcn::SelectionEvent& selectionEvent);

	private:
		void saveWeapons();
		void loadWeapons();
		unsigned int findWeaponType(WeaponType* wt);
};


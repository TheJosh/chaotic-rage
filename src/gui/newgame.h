// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <guichan.hpp>
#include "../rage.h"

using namespace std;


class GametypeListModel;
class MapRegistryListModel;
class VectorListModel;
class GameSettings;


/**
* Dialog for starting a new game - called by the menu
**/
class DialogNewGame : public Dialog, public gcn::ActionListener {
	public:
		DialogNewGame(int num_local, ModManager *mm);
		~DialogNewGame();
		
	protected:
		int num_local;
		GameSettings* gs;
		
	private:
		GametypeListModel *gametype_model;
		MapRegistryListModel *map_model;
		VectorListModel *unittype_model; 
		VectorListModel *viewmode_model;
		
		gcn::DropDown *gametype;
		gcn::DropDown *map;
		gcn::DropDown *unittype;
		gcn::DropDown *viewmode;
		gcn::CheckBox *host;
		
	public:
		virtual gcn::Container * setup();
		virtual void action(const gcn::ActionEvent& actionEvent);
};


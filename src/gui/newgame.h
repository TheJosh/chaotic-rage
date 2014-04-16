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
class DialogNewGame_Action_Weapons;


/**
* Dialog for starting a new game - called by the menu
**/
class DialogNewGame : public Dialog, public gcn::ActionListener {
	friend class DialogNewGame_Action_Weapons;
	
	public:
		DialogNewGame(int num_local);
		virtual ~DialogNewGame();
		
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
		
		DialogNewGame_Action_Weapons *action_weapons;
		
	public:
		virtual gcn::Container * setup();
		virtual void action(const gcn::ActionEvent& actionEvent);
};


/**
* Weapons action
**/
class DialogNewGame_Action_Weapons : public gcn::ActionListener {
	private:
		DialogNewGame *parent;
	public:
		DialogNewGame_Action_Weapons(DialogNewGame *parent): parent(parent) {}
		virtual void action(const gcn::ActionEvent& actionEvent);
};


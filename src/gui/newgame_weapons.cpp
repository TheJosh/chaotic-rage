// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <math.h>
#include <guichan.hpp>

#include "../rage.h"
#include "../render_opengl/menu.h"
#include "../game_manager.h"
#include "../game_settings.h"
#include "../http/serverlist.h"
#include "../mod/mod_manager.h"
#include "dialog.h"
#include "list_models.h"
#include "newgame_weapons.h"

using namespace std;


/**
* Sub-dialog from the new game dialog for mucking about with weapons
**/
DialogNewGameWeapons::DialogNewGameWeapons(DialogNewGame *parent, GameSettings *gs, GameType* gt) : Dialog()
{
	this->parent = parent;
	this->gs = gs;
	this->factions_list = new GametypeFactionsListModel(gt);
	this->faction = -1;
}

DialogNewGameWeapons::~DialogNewGameWeapons()
{
	delete(this->factions_list);
}


#define LEFT        10
#define WIDTH       340
#define ROWHEIGHT   27


/**
* Sub-dialog from the new game dialog for mucking about with weapons
**/
gcn::Container * DialogNewGameWeapons::setup()
{
	gcn::Button* button;
	int y = 10;
	
	c = new gcn::Window("Weapon Settings");
	c->setDimension(gcn::Rectangle(0, 0, 360, 300));
	
	dd_faction = new gcn::DropDown(this->factions_list);
	dd_faction->setPosition(LEFT, y);
	dd_faction->setWidth(WIDTH);
	dd_faction->addSelectionListener(this);
	c->add(dd_faction);
	
	y += ROWHEIGHT;
	
	chk_unit = new gcn::CheckBox("Standard unit weapons");
	chk_unit->setPosition(LEFT, y);
	chk_unit->setWidth(WIDTH);
	c->add(chk_unit);
	
	y += ROWHEIGHT;
	
	chk_gametype = new gcn::CheckBox("Stadard game type weapons");
	chk_gametype->setPosition(LEFT, y);
	chk_gametype->setWidth(WIDTH);
	c->add(chk_gametype);
	
	y += ROWHEIGHT;
	
	button = new gcn::Button("Save Weapon Settings");
	button->setPosition((360 - button->getWidth()) / 2, 250);
	button->addActionListener(this);
	c->add(button);
	
	this->valueChanged(NULL);
	
	return c;
}


/**
* Button click to close and go back to new game dialog
**/
void DialogNewGameWeapons::action(const gcn::ActionEvent& actionEvent)
{
	this->m->remDialog(this);
}


/**
* Handle changes to the faction dropdown
**/
void DialogNewGameWeapons::valueChanged(const gcn::SelectionEvent& selectionEvent)
{
	int new_faction;
	
	// Save old
	if (faction != -1) {
		gs->factions[faction].spawn_weapons_unit = chk_unit->isSelected();
		gs->factions[faction].spawn_weapons_gametype = chk_gametype->isSelected();
	}
	
	// Load new
	new_faction = this->dd_faction->getSelected();
	chk_unit->setSelected(gs->factions[new_faction].spawn_weapons_unit);
	chk_gametype->setSelected(gs->factions[new_faction].spawn_weapons_gametype);
	
	// Update internal variable
	faction = new_faction;
}



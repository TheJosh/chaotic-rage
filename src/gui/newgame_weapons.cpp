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
#include "../game_engine.h"
#include "../http/serverlist.h"
#include "../mod/mod_manager.h"
#include "../mod/weapontype.h"
#include "../i18n/gettext.h"
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
	this->faction = 0;
	this->wts = GEng()->mm->getAllWeaponTypes();
	this->dd_faction = NULL;
	this->chk_gametype = NULL;
	this->chk_unit = NULL;
}

DialogNewGameWeapons::~DialogNewGameWeapons()
{
	delete(this->factions_list);
	delete(this->wts);

	delete(this->dd_faction);
	delete(this->chk_gametype);
	delete(this->chk_unit);

	for (unsigned int i = 0; i < this->chk_custom.size(); i++) {
		delete(this->chk_custom[i]);
	}
}


#define LEFT        10
#define WIDTH       340
#define ROWHEIGHT   21


/**
* Sub-dialog from the new game dialog for mucking about with weapons
**/
gcn::Container * DialogNewGameWeapons::setup()
{
	gcn::Button* button;
	int y = 10;

	c = new gcn::Window(_(STRING_NEWGAME_WEAPONS));
	c->setDimension(gcn::Rectangle(0, 0, 360, 300));

	// Dropdown for choosing which faction you are editing
	dd_faction = new gcn::DropDown(this->factions_list);
	dd_faction->setPosition(LEFT, y);
	dd_faction->setWidth(WIDTH);
	dd_faction->addSelectionListener(this);
	c->add(dd_faction);
	y += ROWHEIGHT + 5;

	// Per-unit weapons
	chk_unit = new gcn::CheckBox(_(STRING_WEAPONS_STANDARD_UNIT));
	chk_unit->setPosition(LEFT, y);
	chk_unit->setWidth(WIDTH);
	c->add(chk_unit);
	y += ROWHEIGHT;

	// Per-gametype weapons
	chk_gametype = new gcn::CheckBox(_(STRING_WEAPONS_STANDARD_GAME));
	chk_gametype->setPosition(LEFT, y);
	chk_gametype->setWidth(WIDTH);
	c->add(chk_gametype);
	y += ROWHEIGHT;

	// Create one checkbox for each weapon type
	for (vector<WeaponType*>::iterator it = this->wts->begin(); it != this->wts->end(); ++it) {
		gcn::CheckBox* checkbox = new gcn::CheckBox((*it)->title);
		checkbox->setPosition(LEFT, y);
		checkbox->setWidth(WIDTH);
		c->add(checkbox);
		chk_custom.push_back(checkbox);
		y += ROWHEIGHT;
	}

	// Save btn
	y += 5;
	button = new gcn::Button(_(STRING_WEAPONS_SAVE));
	button->setPosition((360 - button->getWidth()) / 2, y);
	button->addActionListener(this);
	c->add(button);
	y += ROWHEIGHT;

	// Make window big enough
	c->setDimension(gcn::Rectangle(0, 0, 360, y + 30));

	// Set the checkboxes to match the GameSettings object
	this->loadWeapons();

	return c;
}


/**
* Button click to close and go back to new game dialog
**/
void DialogNewGameWeapons::action(const gcn::ActionEvent& actionEvent)
{
	this->saveWeapons();
	this->m->remDialog(this);
}


/**
* Handle changes to the faction dropdown
**/
void DialogNewGameWeapons::valueChanged(const gcn::SelectionEvent& selectionEvent)
{
	this->saveWeapons();
	faction = this->dd_faction->getSelected();
	assert(faction < NUM_FACTIONS);
	this->loadWeapons();
}


/**
* Save settings from the checkboxes into the GameSettings object for the currently selected faction
**/
void DialogNewGameWeapons::saveWeapons()
{
	unsigned int i;
	assert(this->wts != NULL);

	gs->factions[faction].spawn_weapons_unit = chk_unit->isSelected();
	gs->factions[faction].spawn_weapons_gametype = chk_gametype->isSelected();

	// Save custom weapons
	gs->factions[faction].spawn_weapons_extra.clear();
	for (i = 0; i < this->chk_custom.size(); i++) {
		if (this->chk_custom[i]->isSelected()) {
			gs->factions[faction].spawn_weapons_extra.push_back(this->wts->at(i));
		}
	}
}


/**
* Load weapons into the checkboxes for the currently selected faction from the GameSettings object
**/
void DialogNewGameWeapons::loadWeapons()
{
	unsigned int i;

	chk_unit->setSelected(gs->factions[faction].spawn_weapons_unit);
	chk_gametype->setSelected(gs->factions[faction].spawn_weapons_gametype);

	// Un-check all custom weapons
	for (i = 0; i < this->chk_custom.size(); i++) {
		this->chk_custom[i]->setSelected(false);
	}

	// Re-check the ones in the GameSettingsFaction array
	for (i = 0; i < gs->factions[faction].spawn_weapons_extra.size(); i++) {
		unsigned int chkIdx = this->findWeaponType(gs->factions[faction].spawn_weapons_extra[i]);
		if (chkIdx < this->chk_custom.size()) {
			this->chk_custom[chkIdx]->setSelected(true);
		}
	}
}


/**
* Return the index of a given weapon in the weapons array
**/
unsigned int DialogNewGameWeapons::findWeaponType(WeaponType* wt)
{
	for (unsigned int i = 0; i < this->wts->size(); i++) {
		if (this->wts->at(i) == wt) {
			return i;
		}
	}

	return std::numeric_limits<unsigned int>::max();
}




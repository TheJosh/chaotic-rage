// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <math.h>
#include <guichan.hpp>

#include "../rage.h"
#include "../i18n/gettext.h"
#include "../render_opengl/menu.h"
#include "../game_manager.h"
#include "../game_settings.h"
#include "../game_engine.h"
#include "../http/serverlist.h"
#include "../mod/mod_manager.h"
#include "dialog.h"
#include "list_models.h"
#include "newgame.h"
#include "newgame_weapons.h"
#include "newgame_environment.h"


using namespace std;


/**
* Constructor for "new game" dialog
**/
DialogNewGame::DialogNewGame(int num_local) : Dialog()
{
	this->num_local = num_local;

	this->gametype_model = new GametypeListModel(GEng()->mm->getAllGameTypes());

	this->action_weapons = new DialogNewGame_Action_Weapons(this);
	this->action_environment = new DialogNewGame_Action_Environment(this);

	this->gs = new GameSettings();
	
	// TODO: This dialog should allow you you change various game settings
}

DialogNewGame::~DialogNewGame()
{
	delete(this->gametype_model);
	delete(this->map_model);
	delete(this->unittype_model);
	delete(this->viewmode_model);
	delete(this->action_weapons);
	delete(this->action_environment);
	delete(this->gs);
}


#define COLLEFT     10 * GEng()->gui_scale
#define COLRIGHT    110 * GEng()->gui_scale
#define COLRIGHTW   195 * GEng()->gui_scale
#define ROWHEIGHT   27 * GEng()->gui_scale
#define SMLHEIGHT   23 * GEng()->gui_scale
#define BTNHEIGHT   37 * GEng()->gui_scale


/**
* Setup routine for the "New Game" dialog
**/
gcn::Container * DialogNewGame::setup()
{
	gcn::Button* button;
	gcn::Label* label;
	int y = 10;

	// This is a bit crap. We can't use ->gm until after the dialog is created
	this->map_model = new MapRegistryListModel(this->gm->getMapRegistry());
	this->unittype_model = new VectorListModel(this->gm->getUnitTypes());
	this->viewmode_model = new VectorListModel(this->gm->getViewModes());


	if (this->num_local == 1) {
		c = new gcn::Window(_(STRING_MENU_SINGLE));
	} else if (this->num_local > 1) {
		c = new gcn::Window(_(STRING_MENU_SPLIT));
	}

	c->setDimension(gcn::Rectangle(0, 0, 320 * GEng()->gui_scale, 280 * GEng()->gui_scale));


	label = new gcn::Label(_(STRING_NEWGAME_TYPE));
	c->add(label, COLLEFT, y);

	this->gametype = new gcn::DropDown(this->gametype_model);
	this->gametype->setPosition(COLRIGHT, y);
	this->gametype->setWidth(COLRIGHTW);
	c->add(this->gametype);
	y += SMLHEIGHT + 10;

	button = new gcn::Button(_(STRING_NEWGAME_WEAPONS));
	button->setPosition(COLRIGHT, y);
	button->addActionListener(this->action_weapons);
	c->add(button);
	y += BTNHEIGHT - 5;

	button = new gcn::Button(_(STRING_NEWGAME_ENVIRONMENT));
	button->setPosition(COLRIGHT, y);
	button->addActionListener(this->action_environment);
	c->add(button);
	y += BTNHEIGHT;

	label = new gcn::Label(_(STRING_NEWGAME_MAP));
	c->add(label, COLLEFT, y);

	this->map = new gcn::DropDown(this->map_model);
	this->map->setPosition(COLRIGHT, y);
	this->map->setWidth(COLRIGHTW);
	c->add(this->map);

	y += ROWHEIGHT;

	label = new gcn::Label(_(STRING_NEWGAME_UNIT));
	c->add(label, COLLEFT, y);

	this->unittype = new gcn::DropDown(this->unittype_model);
	this->unittype->setPosition(COLRIGHT, y);
	this->unittype->setWidth(COLRIGHTW);
	c->add(this->unittype);

	y += ROWHEIGHT;

	label = new gcn::Label(_(STRING_NEWGAME_VIEW));
	c->add(label, COLLEFT, y);

	this->viewmode = new gcn::DropDown(this->viewmode_model);
	this->viewmode->setPosition(COLRIGHT, y);
	this->viewmode->setWidth(COLRIGHTW);
	c->add(this->viewmode);

	y += ROWHEIGHT;

	this->host = new gcn::CheckBox(_(STRING_NEWGAME_HOST));
	this->host->setPosition(COLRIGHT, y);
	this->host->setWidth(COLRIGHTW);
	c->add(this->host);

	button = new gcn::Button(this->num_local == 1 ? _(STRING_NEWGAME_START_SINGLE) : _(STRING_NEWGAME_START_SPLIT));
	button->setPosition((c->getWidth() - button->getWidth()) / 2, c->getHeight() - button->getHeight() - ROWHEIGHT);
	button->addActionListener(this);
	c->add(button);

	return c;
}

/**
* Button click processing for the "New Game" dialog
**/
void DialogNewGame::action(const gcn::ActionEvent& actionEvent)
{
	this->m->startGame(
		this->gm->getMapRegistry()->at(this->map->getSelected()),
		this->gm->getGameTypes()->at(this->gametype->getSelected()),
		this->gm->getUnitTypes()->at(this->unittype->getSelected()),
		static_cast<GameSettings::ViewMode>(this->viewmode->getSelected()),
		this->num_local,
		this->host->isSelected(),
		this->gs
	);
}


/**
* Button click processing for the "weapons" button
**/
void DialogNewGame_Action_Weapons::action(const gcn::ActionEvent& actionEvent)
{
	string gametype = this->parent->gm->getGameTypes()->at(this->parent->gametype->getSelected());

	GameType* gt = GEng()->mm->getGameType(gametype);
	assert(gt);

	this->parent->m->addDialog(new DialogNewGameWeapons(this->parent, this->parent->gs, gt));
}


/**
* Button click processing for the "environment" button
**/
void DialogNewGame_Action_Environment::action(const gcn::ActionEvent& actionEvent)
{
	this->parent->m->addDialog(new DialogNewGameEnvironment(this->parent, this->parent->gs));
}


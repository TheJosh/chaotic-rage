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
#include "newgame.h"
#include "newgame_weapons.h"


using namespace std;


/**
* Constructor for "new game" dialog
**/
DialogNewGame::DialogNewGame(int num_local, ModManager *mm) : Dialog()
{
	this->num_local = num_local;
	
	this->gametype_model = new GametypeListModel(mm->getAllGameTypes());
	
	this->action_weapons = new DialogNewGame_Action_Weapons(this);
	
	this->gs = new GameSettings();
}

DialogNewGame::~DialogNewGame()
{
	delete(this->gametype_model);
	delete(this->map_model);
	delete(this->unittype_model);
	delete(this->viewmode_model);
	delete(this->action_weapons);
	delete(this->gs);
}


#define COLLEFT     10
#define COLRIGHT    110
#define COLRIGHTW   195
#define ROWHEIGHT   27
#define SMLHEIGHT   23
#define BTNHEIGHT   37


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
		c = new gcn::Window("Single Player");
	} else if (this->num_local > 1) {
		c = new gcn::Window("Split Screen");
	}
	
	c->setDimension(gcn::Rectangle(0, 0, 320, 250));
	
	
	label = new gcn::Label("Game type");
	c->add(label, COLLEFT, y);
	
	this->gametype = new gcn::DropDown(this->gametype_model);
	this->gametype->setPosition(COLRIGHT, y);
	this->gametype->setWidth(COLRIGHTW);
	c->add(this->gametype);
	
	y += SMLHEIGHT;
	
	button = new gcn::Button("Weapons");
	button->setPosition(COLRIGHT, y);
	button->addActionListener(this->action_weapons);
	c->add(button);
	
	y += BTNHEIGHT;
	
	label = new gcn::Label("Map");
	c->add(label, COLLEFT, y);
	
	this->map = new gcn::DropDown(this->map_model);
	this->map->setPosition(COLRIGHT, y);
	this->map->setWidth(COLRIGHTW);
	c->add(this->map);
	
	y += ROWHEIGHT;
	
	label = new gcn::Label("Unit type");
	c->add(label, COLLEFT, y);
	
	this->unittype = new gcn::DropDown(this->unittype_model);
	this->unittype->setPosition(COLRIGHT, y);
	this->unittype->setWidth(COLRIGHTW);
	c->add(this->unittype);
	
	y += ROWHEIGHT;
	
	label = new gcn::Label("View mode");
	c->add(label, COLLEFT, y);
	
	this->viewmode = new gcn::DropDown(this->viewmode_model);
	this->viewmode->setPosition(COLRIGHT, y);
	this->viewmode->setWidth(COLRIGHTW);
	c->add(this->viewmode);
	
	y += ROWHEIGHT;
	
	this->host = new gcn::CheckBox("Host network game");
	this->host->setPosition(COLRIGHT, y);
	this->host->setWidth(COLRIGHTW);
	c->add(this->host);
	
	y += ROWHEIGHT;
	
	button = new gcn::Button(this->num_local == 1 ? "Start Single Player Game" : "Start Split Screen Game");
	button->setPosition((320 - button->getWidth()) / 2, 200);
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
		this->viewmode->getSelected(),
		this->num_local,
		this->host->isSelected(),
		this->gs
	);
}


/**
* Button click processing for the "New Game" dialog
**/
void DialogNewGame_Action_Weapons::action(const gcn::ActionEvent& actionEvent)
{
	this->parent->m->addDialog(new DialogNewGameWeapons(this->parent, this->parent->gs));
}



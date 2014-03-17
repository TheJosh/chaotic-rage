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


using namespace std;


/**
* Constructor for "new game" dialog
**/
DialogNewGame::DialogNewGame(int num_local, ModManager *mm)
{
	this->num_local = num_local;
	
	this->gametype_model = new GametypeListModel(mm->getAllGameTypes());
	
	this->gs = new GameSettings();
}

DialogNewGame::~DialogNewGame()
{
	delete(this->gametype_model);
	delete(this->map_model);
	delete(this->unittype_model);
	delete(this->viewmode_model);
	delete(this->gs);
}


/**
* Setup routine for the "New Game" dialog
**/
gcn::Container * DialogNewGame::setup()
{
	gcn::Button* button;
	gcn::Label* label;
	
	
	this->map_model = new MapRegistryListModel(this->gm->getMapRegistry());
	this->unittype_model = new VectorListModel(this->gm->getUnitTypes());
	this->viewmode_model = new VectorListModel(this->gm->getViewModes());
	
	
	if (this->num_local == 1) {
		c = new gcn::Window("Single Player");
	} else if (this->num_local > 1) {
		c = new gcn::Window("Split Screen");
	}
	
	c->setDimension(gcn::Rectangle(0, 0, 255, 200));
	
	
	label = new gcn::Label("Game type");
	c->add(label, 10, 10);
	
	this->gametype = new gcn::DropDown(this->gametype_model);
	this->gametype->setPosition(80, 10);
	this->gametype->setWidth(160);
	c->add(this->gametype);
	
	
	label = new gcn::Label("Map");
	c->add(label, 10, 30);
	
	this->map = new gcn::DropDown(this->map_model);
	this->map->setPosition(80, 30);
	this->map->setWidth(160);
	c->add(this->map);
	
	
	label = new gcn::Label("Unit type");
	c->add(label, 10, 50);
	
	this->unittype = new gcn::DropDown(this->unittype_model);
	this->unittype->setPosition(80, 50);
	this->unittype->setWidth(160);
	c->add(this->unittype);
	
	
	label = new gcn::Label("View mode");
	c->add(label, 10, 70);
	
	this->viewmode = new gcn::DropDown(this->viewmode_model);
	this->viewmode->setPosition(80, 70);
	this->viewmode->setWidth(160);
	c->add(this->viewmode);
	
	
	this->host = new gcn::CheckBox("Host network game");
	this->host->setPosition(80, 100);
	this->host->setWidth(160);
	c->add(this->host);
	
	
	button = new gcn::Button("Start Game");
	button->setPosition((255 - button->getWidth()) / 2, 150);
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


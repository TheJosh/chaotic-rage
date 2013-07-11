// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include <math.h>
#include <guichan.hpp>
#include <guichan/sdl.hpp>
#include <guichan/opengl.hpp>
#include <guichan/opengl/openglsdlimageloader.hpp>

#include "../rage.h"
#include "../menu.h"
#include "../http/serverlist.h"
#include "../mod/mod_manager.h"
#include "dialog.h"
#include "newgame.h"


using namespace std;


/**
* Constructor for "new game" dialog
**/
DialogNewGame::DialogNewGame(int num_local, ModManager *mm)
{
	this->num_local = num_local;
	
	this->gametypes = new GametypeListModel(mm->getAllGameTypes());
}

DialogNewGame::~DialogNewGame()
{
	delete(this->gametypes);
}


/**
* Setup routine for the "New Game" dialog
**/
gcn::Container * DialogNewGame::setup()
{
	gcn::Button* button;
	gcn::Label* label;
	
	if (this->num_local == 1) {
		c = new gcn::Window("Single Player");
	} else if (this->num_local > 1) {
		c = new gcn::Window("Split Screen");
	}
	
	c->setDimension(gcn::Rectangle(0, 0, 255, 200));
	
	
	label = new gcn::Label("Game type");
	c->add(label, 10, 10);
	
	this->gametype = new gcn::DropDown(this->gametypes);
	this->gametype->setPosition(80, 10);
	this->gametype->setWidth(160);
	c->add(this->gametype);
	
	
	label = new gcn::Label("Map");
	c->add(label, 10, 30);
	
	this->map = new gcn::DropDown(new MapRegistryListModel(this->m->mapreg));
	this->map->setPosition(80, 30);
	this->map->setWidth(160);
	c->add(this->map);
	
	
	label = new gcn::Label("Unit type");
	c->add(label, 10, 50);
	
	this->unittype = new gcn::DropDown(new VectorListModel(&this->m->unittypes));
	this->unittype->setPosition(80, 50);
	this->unittype->setWidth(160);
	c->add(this->unittype);
	
	
	label = new gcn::Label("View mode");
	c->add(label, 10, 70);
	
	this->viewmode = new gcn::DropDown(new VectorListModel(&this->m->viewmodes));
	this->viewmode->setPosition(80, 70);
	this->viewmode->setWidth(160);
	c->add(this->viewmode);
	
	
	button = new gcn::Button("Start Game");
	button->setPosition(120, 150);
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
		this->m->mapreg->at(this->map->getSelected()),
		this->m->gametypes[this->gametype->getSelected()],
		this->m->unittypes[this->unittype->getSelected()],
		this->viewmode->getSelected(),
		this->num_local
	);
}


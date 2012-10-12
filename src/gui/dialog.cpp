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
#include "dialog.h"


using namespace std;


/**
* Constructor for "new game" dialog
**/
DialogNewGame::DialogNewGame(int num_local)
{
	this->num_local = num_local;
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
	
	c->setDimension(gcn::Rectangle(0, 0, 200, 200));
	
	
	label = new gcn::Label("Game type");
	c->add(label, 10, 10);
	
	this->gametype = new gcn::DropDown(new VectorListModel(&this->m->gametypes));
	this->gametype->setPosition(80, 10);
	c->add(this->gametype);
	
	
	label = new gcn::Label("Map");
	c->add(label, 10, 30);
	
	this->map = new gcn::DropDown(new MapRegistryListModel(this->m->mapreg));
	this->map->setPosition(80, 30);
	c->add(this->map);
	
	
	label = new gcn::Label("Unit type");
	c->add(label, 10, 50);
	
	this->unittype = new gcn::DropDown(new VectorListModel(&this->m->unittypes));
	this->unittype->setPosition(80, 50);
	c->add(this->unittype);
	
	
	label = new gcn::Label("View mode");
	c->add(label, 10, 70);
	
	this->viewmode = new gcn::DropDown(new VectorListModel(&this->m->viewmodes));
	this->viewmode->setPosition(80, 70);
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
		this->m->mapreg->nameAt(this->map->getSelected()),
		this->m->gametypes[this->gametype->getSelected()],
		this->m->unittypes[this->unittype->getSelected()],
		this->viewmode->getSelected(),
		this->num_local
	);
}



/**
* Setup routine for the NULL dialog
**/
gcn::Container * DialogNull::setup()
{
	gcn::Label* label;

	c = new gcn::Window("Argh");
	c->setDimension(gcn::Rectangle(0, 0, 300, 100));
	
	label = new gcn::Label("This action is not available at this time");
	c->add(label, 22, 20);
	
	return c;
}


/**
* Setup routine for the NULL dialog
**/
gcn::Container * DialogControls::setup()
{
	string controls_single[] = {
		"Move", "W A S D",
		"Aim", "Mouse",
		"Fire", "Left Click",
		"Melee", "Right Click",
		"Change Weapon", "Scroll",
		"Use", "E",
		"Lift", "Q",
		"Special", "T",
		""
	};
	
	string controls_split1[] = {
		"Move", "W A S D",
		"Aim", "O P 9 L",
		"Fire", "K",
		"Melee", "U",
		"Change Weapon", " ",
		"Use", "E",
		"Lift", "Q",
		"Special", "T",
		""
	};
	
	string controls_split2[] = {
		"Move", "Numpad 4 5 6 8",
		"Aim", "Mouse",
		"Fire", "Left Click",
		"Melee", "Right Click",
		"Change Weapon", " ",
		"Use", "Numpad 9",
		"Lift", "Numpad 7",
		"Special", "Numpad 1",
		""
	};
	
	int cols[] = {22, 200, 222, 400, 422, 600};
	
	
	gcn::Label* label;
	
	c = new gcn::Window("Controls");
	c->setDimension(gcn::Rectangle(0, 0, 622, 100));
	
	label = new gcn::Label("Single Player");
	label->setAlignment(gcn::Graphics::CENTER);
	label->setWidth(178);
	c->add(label, cols[0], 15);
	
	label = new gcn::Label("Split Screen Player 1");
	label->setAlignment(gcn::Graphics::CENTER);
	label->setWidth(178);
	c->add(label, cols[2], 15);
	
	label = new gcn::Label("Split Screen Player 2");
	label->setAlignment(gcn::Graphics::CENTER);
	label->setWidth(178);
	c->add(label, cols[4], 15);
	
	int y = 40;
	int i = 0;
	do {
		if (controls_single[i].length() == 0) break;
		
		label = new gcn::Label(controls_single[i]);
		c->add(label, cols[0], y);
		
		label = new gcn::Label(controls_split1[i]);
		c->add(label, cols[2], y);
		
		label = new gcn::Label(controls_split2[i]);
		c->add(label, cols[4], y);
		
		i++;
		
		label = new gcn::Label(controls_single[i]);
		label->setAlignment(gcn::Graphics::RIGHT);
		c->add(label, cols[1] - label->getWidth(), y);
		
		label = new gcn::Label(controls_split1[i]);
		label->setAlignment(gcn::Graphics::RIGHT);
		c->add(label, cols[3] - label->getWidth(), y);
		
		label = new gcn::Label(controls_split2[i]);
		label->setAlignment(gcn::Graphics::RIGHT);
		c->add(label, cols[5] - label->getWidth(), y);
		
		i++;
		
		y += 17;
	} while(1);
	
	c->setHeight(y + 30);
	
	return c;
}



/**
* Quit dialog is for in-game quitting
**/
DialogQuit::DialogQuit(GameState *st)
{
	this->st = st;
}


/**
* Setup routine for the NULL dialog
**/
gcn::Container * DialogQuit::setup()
{
	gcn::Button* button;
	
	c = new gcn::Window("Quit? Really?");
	c->setDimension(gcn::Rectangle(0, 0, 300, 100));
	
	button = new gcn::Button("Yes");
	button->setPosition(150 - 5 - 60, 30);
	button->setWidth(60);
	button->setId("Y");
	button->addActionListener(this);
	c->add(button);
	
	button = new gcn::Button("No");
	button->setPosition(150 + 5, 30);
	button->setWidth(60);
	button->setId("N");
	button->addActionListener(this);
	c->add(button);
	
	return c;
}

/**
* Handle a button click
**/
void DialogQuit::action(const gcn::ActionEvent& actionEvent)
{
	if (actionEvent.getSource()->getId() == "Y") {
		this->st->running = false;
	}
	this->st->remDialog(this);
}


/**
* Dialog for mod selection
**/
DialogMods::DialogMods()
{
}


/**
* Setup routine for the mods dialog
**/
gcn::Container * DialogMods::setup()
{
	const int w = 300;	// width
	const int h = 500;	// height
	const int p = 10;	// padding
	const int bw = 60;	// buttonwidth
	const int bh = 20;	// buttonheight

	gcn::Button* button;
	gcn::CheckBox* chk;

	c = new gcn::Window("Mod selector");
	c->setDimension(gcn::Rectangle(0, 0, w, h));
	
	vector<string> * avail = m->mm->getAvailMods();
	int y = p;
	for (vector<string>::iterator it = avail->begin(); it != avail->end(); it++) {
		chk = new gcn::CheckBox(*it);
		chk->adjustSize();
		chk->setPosition(p, y);
		c->add(chk);
		y += chk->getHeight() + p;
	}
	delete(avail);

	button = new gcn::Button("Save");
	button->setPosition(w - bw - p, y);
	button->setSize(bw, bh);
	button->setId("S");
	button->addActionListener(this);
	c->add(button);
	
	c->setHeight(y + bh + p + 15);

	return c;
}

/**
* Handle a button click
**/
void DialogMods::action(const gcn::ActionEvent& actionEvent)
{
	if (actionEvent.getSource()->getId() == "S") {
		this->m->setDialog(NULL);
	}
}

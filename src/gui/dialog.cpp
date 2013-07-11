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
#include "../gamestate.h"
#include "dialog.h"


using namespace std;



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
* Quit dialog is for in-game quitting
**/
DialogQuit::DialogQuit(GameState *st)
{
	this->st = st;
}


/**
* Setup routine for the quit dialog
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
		this->st->gameOver();
	}
	this->st->remDialog(this);
}



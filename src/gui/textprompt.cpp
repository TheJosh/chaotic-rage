// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <math.h>
#include <guichan.hpp>

#include "../rage.h"
#include "../game_state.h"
#include "../game_engine.h"
#include "textprompt.h"


using namespace std;



/**
* Allows the user to enter some text
**/
DialogTextPrompt::DialogTextPrompt(GameState *st, string title, string message, DialogTextPromptHandler* ev)
{
	this->st = st;

	this->title = title;
	this->message = message;
	this->ev = ev;

	this->label = NULL;
	this->text = NULL;
	this->button = NULL;
}


/**
* Setup routine for the text prompt dialog
**/
gcn::Container* DialogTextPrompt::setup()
{
	c = new gcn::Window(this->title);
	c->setDimension(gcn::Rectangle(0, 0, 340, 150));

	this->label = new gcn::Label(this->message);
	this->label->setPosition(20, 20);
	this->label->setWidth(300);
	c->add(this->label);

	this->text = new gcn::TextField("");
	this->text->setPosition(20, 50);
	this->text->setWidth(300);
	c->add(this->text);

	this->button = new gcn::Button("OK");
	this->button->setPosition(150, 90);
	this->button->setWidth(50);
	this->button->addActionListener(this);
	c->add(this->button);

	return c;
}


/**
* Cleanup the text prompt dialog
**/
DialogTextPrompt::~DialogTextPrompt()
{
	// TODO: Bring these back. It was crashing on the ->remDialog(this); below
	// We probably need delayed remove like we have with entities
	//delete button;
	//delete text;
	//delete label;

	delete ev;
}


/**
* Handle a button click
**/
void DialogTextPrompt::action(const gcn::ActionEvent& actionEvent)
{
	if (this->ev != NULL) {
		this->ev->handleText(this->text->getText());
	}

	GEng()->remDialog(this);
}



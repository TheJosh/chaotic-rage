// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <math.h>
#include <guichan.hpp>

#include "../game_state.h"
#include "../game_engine.h"
#include "listprompt.h"
#include "list_models.h"


using namespace std;



/**
* Allows the user to enter some text
* The `items` and `ev` will be FREED when this dialog is deleted
*
* TODO: Is passing a vector the best thing here, or should we pass a ListModel instead?
**/
DialogListPrompt::DialogListPrompt(string title, string message, vector<string>* items, DialogListPromptHandler* ev)
{
	this->title = title;
	this->message = message;
	this->items = items;
	this->items_model = new VectorListModel(items);
	this->ev = ev;
	
	this->label = NULL;
	this->list = NULL;
	this->button = NULL;
}


/**
* Setup routine for the text prompt dialog
**/
gcn::Container* DialogListPrompt::setup()
{
	c = new gcn::Window(this->title);
	c->setDimension(gcn::Rectangle(0, 0, 340, 150));

	this->label = new gcn::Label(this->message);
	this->label->setPosition(20, 20);
	this->label->setWidth(300);
	c->add(this->label);

	this->list = new gcn::DropDown(this->items_model);
	this->list->setPosition(20, 50);
	this->list->setWidth(300);
	c->add(this->list);

	this->button = new gcn::Button("OK");
	this->button->setPosition(150, 80);
	this->button->setWidth(50);
	this->button->addActionListener(this);
	c->add(this->button);

	return c;
}


/**
* Destroy child widgets created in the setup() method.
* Called before the container is removed from the top-level guichan container.
* Don't remove the actual container though!
**/
void DialogListPrompt::tearDown()
{
	delete button;
	delete list;
	delete label;
}


/**
* Cleanup the text prompt dialog
**/
DialogListPrompt::~DialogListPrompt()
{
	delete this->items_model;
	delete items;
	delete ev;
}


/**
* Handle a button click
**/
void DialogListPrompt::action(const gcn::ActionEvent& actionEvent)
{
	if (this->ev != NULL) {
		this->ev->handleSelection(this->list->getSelected());
	}
}

// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <math.h>
#include <guichan.hpp>

#include "../rage.h"
#include "../game_state.h"
#include "../game_engine.h"
#include "buttonbar.h"


using namespace std;



/**
* Shows a bunch of buttons
**/
DialogButtonBar::DialogButtonBar(string title, vector<string>& labels, DialogButtonBarHandler* ev)
{
	this->title = title;
	this->ev = ev;

	for (vector<string>::iterator it = labels.begin(); it != labels.end(); ++it) {
		this->buttons.push_back(new gcn::Button(*it));
	}
}


/**
* Setup routine for the text prompt dialog
**/
gcn::Container* DialogButtonBar::setup()
{
	int y = 10;

	c = new gcn::Window(this->title);
	c->setDimension(gcn::Rectangle(0, 0, 220, this->buttons.size() * 40 + 10));
	
	// Add each button
	int idx = 0;
	char buf[5];
	for (vector<gcn::Button*>::iterator it = buttons.begin(); it != buttons.end(); ++it) {
		sprintf(buf, "%i", idx);

		gcn::Button* btn = *it;
		btn->setSize(200, 30);
		btn->setPosition(10, y);
		btn->setId(buf);
		c->add(btn);

		++idx;
		y += 40;
	}
	
	return c;
}


/**
* Destroy child widgets created in the setup() method.
* Called before the container is removed from the top-level guichan container.
* Don't remove the actual container though!
**/
void DialogButtonBar::tearDown()
{
	for (vector<gcn::Button*>::iterator it = buttons.begin(); it != buttons.end(); ++it) {
		c->remove(*it);
		delete *it;
	}
}


/**
* Cleanup the text prompt dialog
**/
DialogButtonBar::~DialogButtonBar()
{
	delete ev;
}


/**
* Handle a button click
**/
void DialogButtonBar::action(const gcn::ActionEvent& actionEvent)
{
	if (this->ev != NULL) {
		int index = atoi(actionEvent.getSource()->getId().c_str());
		this->ev->handleButton(index);
	}
}



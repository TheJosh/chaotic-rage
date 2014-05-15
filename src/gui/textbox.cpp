// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <math.h>
#include <guichan.hpp>

#include "../rage.h"
#include "../game_state.h"
#include "../game_engine.h"
#include "textbox.h"


using namespace std;



/**
* A multi-line text box window
**/
DialogTextBox::DialogTextBox(GameState *st, string title)
{
	this->st = st;
	this->title = title;
	
	this->text = NULL;
}


/**
* Setup routine for the text prompt dialog
**/
gcn::Container* DialogTextBox::setup()
{
	c = new gcn::Window(this->title);
	c->setDimension(gcn::Rectangle(0, 0, 340, 450));
	
	this->text = new gcn::TextBox();
	this->scroller = new gcn::ScrollArea(this->text);
	this->scroller->setPosition(20, 20);
	this->scroller->setSize(300, 400);
	c->add(this->scroller);
	
	return c;
}


/**
* Cleanup the text prompt dialog
**/
DialogTextBox::~DialogTextBox()
{
	// TODO: Bring these back. It was crashing on the ->remDialog(this); below
	// We probably need delayed remove like we have with entities
	//delete text;
}


/**
* Get the text of this box
**/
string DialogTextBox::getText() const
{
	return this->text->getText();
}


/**
* Set the text of this box
**/
void DialogTextBox::setText(string text)
{
	this->text->setText(text);
}


/**
* Append a line to the end of the text box
**/
void DialogTextBox::appendLine(const string &text)
{
	this->text->addRow(text);
}


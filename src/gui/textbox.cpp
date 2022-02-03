// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <math.h>
#include <guichan.hpp>
#include <SDL.h>

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
	this->btnCopy = NULL;
	this->btnPaste = NULL;
	this->btnClose = NULL;
}


/**
* Setup routine for the text prompt dialog
**/
gcn::Container* DialogTextBox::setup()
{
	c = new gcn::Window(this->title);
	c->setDimension(gcn::Rectangle(0, 0, 340, 400));

	// Main text field
	this->text = new gcn::TextBox();
	this->scroller = new gcn::ScrollArea(this->text);
	this->scroller->setPosition(20, 20);
	this->scroller->setSize(300, 320);
	c->add(this->scroller);

	// Button to copy to clipboard
	this->btnCopy = new gcn::Button("Copy");
	this->btnCopy->setPosition(20, 350);
	this->btnCopy->setId("c");
	this->btnCopy->addActionListener(this);
	c->add(this->btnCopy);

	// Button to paste from clipboard
	this->btnPaste = new gcn::Button("Paste");
	this->btnPaste->setPosition(100, 350);
	this->btnPaste->setId("p");
	this->btnPaste->addActionListener(this);
	c->add(this->btnPaste);

	// Close the dialog
	this->btnClose = new gcn::Button("Close");
	this->btnClose->setPosition(170, 350);
	this->btnClose->setId("x");
	this->btnClose->addActionListener(this);
	c->add(this->btnClose);

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
* Handle actions
**/
void DialogTextBox::action(const gcn::ActionEvent& actionEvent)
{
	if (actionEvent.getSource()->getId() == "c") {
		SDL_SetClipboardText(this->text->getText().c_str());

	} else if (actionEvent.getSource()->getId() == "p") {
		char* text = SDL_GetClipboardText();
		if (text != NULL) {
			this->text->setText(std::string(text));
		}

	} else if (actionEvent.getSource()->getId() == "x") {
		this->close();
	}
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

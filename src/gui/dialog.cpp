// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include "dialog.h"
#include "../game_engine.h"
#include "../game_state.h"
#include "../i18n/gettext.h"


using namespace std;


/**
* Dialog cleanup
**/
Dialog::~Dialog()
{
}


/**
* Return the base container for a dialog
**/
gcn::Container* Dialog::getContainer()
{
	return this->c;
}


/**
* Close a dialog
**/
void Dialog::close()
{
	GEng()->remDialog(this);
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
* Quit dialog is for in-game quitting
**/
DialogQuit::DialogQuit(GameState *st)
{
	this->st = st;
}


#define WIN_WIDTH 300 * GEng()->gui_scale
#define WIN_HEIGHT 100 * GEng()->gui_scale
#define BTN_WIDTH 60 * GEng()->gui_scale
#define BTN_HEIGHT 30 * GEng()->gui_scale

/**
* Setup routine for the quit dialog
**/
gcn::Container * DialogQuit::setup()
{
	gcn::Button* button;

	c = new gcn::Window(_(STRING_GAME_QUIT));
	c->setDimension(gcn::Rectangle(0, 0, WIN_WIDTH, WIN_HEIGHT));

	button = new gcn::Button(_(STRING_GAME_YES));
	button->setPosition(WIN_WIDTH/2 - 5 - BTN_WIDTH, BTN_HEIGHT);
	button->setWidth(BTN_WIDTH);
	button->setId("Y");
	button->addActionListener(this);
	c->add(button);

	button = new gcn::Button(_(STRING_GAME_NO));
	button->setPosition(WIN_WIDTH/2 + 5, BTN_HEIGHT);
	button->setWidth(BTN_WIDTH);
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
		this->st->terminate();
	}

	GEng()->remDialog(this);
}

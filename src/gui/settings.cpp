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
#include "../http/serverlist.h"


using namespace std;



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


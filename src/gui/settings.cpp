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
DialogMods::DialogMods(ModManager* mm)
{
	this->mods = mm->getAvailMods();
}

/**
* Dialog for mod selection
**/
DialogMods::~DialogMods()
{
	delete(this->mods);
}

/**
* Setup routine for the mods dialog
**/
gcn::Container * DialogMods::setup()
{
	const int w = 300;	// width
	const int h = 200;	// height
	const int p = 10;	// padding
	const int bw = 60;	// buttonwidth
	const int bh = 20;	// buttonheight
	
	gcn::Button* button;
	
	c = new gcn::Window("Choose current mod");
	c->setDimension(gcn::Rectangle(0, 0, w, h + 15));
	
	this->modlist = new gcn::ListBox(new VectorListModel(this->mods));
	this->modlist->setPosition(p, p);
	this->modlist->setWidth(w - p - p);
	this->modlist->setHeight(h - bh - p - p - p);
	c->add(this->modlist);
	
	button = new gcn::Button("Save");
	button->setPosition(w - bw - p, h - bh - p);
	button->setSize(bw, bh);
	button->setId("S");
	button->addActionListener(this);
	c->add(button);
	
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


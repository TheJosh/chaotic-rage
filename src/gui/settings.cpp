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
DialogMods::DialogMods(GameState* st, ModManager* mm)
{
	this->st = st;
	this->mm = mm;
	
	vector<string>* modnames = mm->getAvailMods();
	vector<Mod*>* modlist = new vector<Mod*>();
	for (vector<string>::iterator it = modnames->begin(); it != modnames->end(); it++) {
		Mod* m = new Mod(st, "data/" + *it);
		m->loadMetadata();
		modlist->push_back(m);
	}
	delete(modnames);
	
	this->mods = new ModListModel(modlist);
}

/**
* Dialog for mod selection
**/
DialogMods::~DialogMods()
{
	for (int i = this->mods->getNumberOfElements(); i != 0; i++) {
		delete(this->mods->getModAt(i));
	}
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
	
	this->modlist = new gcn::ListBox(this->mods);
	this->modlist->setPosition(p, p);
	this->modlist->setWidth(w - p - p);
	this->modlist->setHeight(h - bh - p - p - p);
	c->add(this->modlist);
	
	button = new gcn::Button("OK");
	button->setPosition(w - bw - p, h - bh - p);
	button->setSize(bw, bh);
	button->addActionListener(this);
	c->add(button);
	
	return c;
}

/**
* Handle a button click
**/
void DialogMods::action(const gcn::ActionEvent& actionEvent)
{
	Mod* newsuppl = NULL;

	// Try to load new mod
	string modname = this->mods->getModAt(this->modlist->getSelected())->getName();
	if (modname != "cr") {
		newsuppl = new Mod(this->st, "data/" + modname);
		if (! newsuppl->load()) {
			reportFatalError("Unable to load mod '" + modname + "'.");
		}
	}

	// If there was a suppl, remove it
	this->mm->remMod(this->mm->getSuppl());
	
	// If there is a new suppl, add it
	if (newsuppl) {
		this->mm->addMod(newsuppl);
		this->mm->setSuppl(newsuppl);
	}
	
	this->m->loadModBits();
	this->m->setDialog(NULL);
}


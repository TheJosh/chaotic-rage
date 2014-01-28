// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <math.h>
#include <guichan.hpp>

#include "../rage.h"
#include "../http/serverlist.h"
#include "../render_opengl/guichan_imageloader.h"
#include "../mod/mod.h"
#include "../mod/mod_manager.h"
#include "../render_opengl/menu.h"
#include "dialog.h"
#include "list_models.h"
#include "mods.h"


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

	this->img = NULL;
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
	delete(this->img);
	delete(this->icon);
}

/**
* Setup routine for the mods dialog
**/
gcn::Container * DialogMods::setup()
{
	const int w = 532;	// width
	const int h = 326;	// height
	const int p = 10;	// padding
	const int bw = 60;	// buttonwidth
	const int bh = 20;	// buttonheight

	c = new gcn::Window("Choose current mod");
	c->setDimension(gcn::Rectangle(0, 0, w, h + 15));
	
	this->modlist = new gcn::DropDown(this->mods);
	this->modlist->setPosition(p, p);
	this->modlist->setWidth(w - p - p);
	this->modlist->setSelected(this->mods->findMod(this->mm->getSupplOrBase()));
	this->modlist->addSelectionListener(this);
	c->add(this->modlist);
	
	this->icon = new gcn::Icon();
	this->icon->setPosition(p, p + 20);
	this->icon->setSize(512, 256);
	c->add(this->icon);
	this->valueChanged(NULL);

	this->button = new gcn::Button("OK");
	this->button->setPosition(w - bw - p, h - bh - p);
	this->button->setSize(bw, bh);
	this->button->addActionListener(this);
	c->add(this->button);
	
	return c;
}


void DialogMods::action(const gcn::ActionEvent& actionEvent)
{
	Mod* newsuppl = NULL;

	// Try to load new mod
	string modname = this->mods->getModAt(this->modlist->getSelected())->getName();
	if (modname != "cr") {
		newsuppl = new Mod(this->st, "data/" + modname);
		if (! newsuppl->load(NULL)) {
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


/**
* Handle a change to the selection
**/
void DialogMods::valueChanged(const gcn::SelectionEvent& selectionEvent)
{
	delete(this->img);
	gcn::ImageLoader* oldloader = gcn::Image::getImageLoader();
	gcn::Image::setImageLoader(new gcn::ChaoticRageOpenGLSDLImageLoader(this->mods->getModAt(this->modlist->getSelected())));
	this->img = gcn::Image::load("menu/mod.jpg", true);
	this->icon->setImage(this->img);
	gcn::Image::setImageLoader(oldloader);
}

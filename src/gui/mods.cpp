// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <math.h>
#include <guichan.hpp>

#include "../rage.h"
#include "../i18n/gettext.h"
#include "../http/serverlist.h"
#include "../render_opengl/guichan_imageloader.h"
#include "../mod/mod.h"
#include "../mod/mod_manager.h"
#include "../game_engine.h"
#include "../render_opengl/menu.h"
#include "dialog.h"
#include "list_models.h"
#include "mods.h"


using namespace std;



/**
* Dialog for mod selection
**/
DialogMods::DialogMods(GameState* st) : Dialog()
{
	this->st = st;

	vector<string>* modnames = GEng()->mm->getAvailMods();
	vector<Mod*>* modlist = new vector<Mod*>();
	for (vector<string>::iterator it = modnames->begin(); it != modnames->end(); ++it) {
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
	for (int i = this->mods->getNumberOfElements() - 1; i >= 0; i--) {
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
	const int w = 532 * GEng()->gui_scale;	// width
	const int h = 326 * GEng()->gui_scale;	// height
	const int p = 10 * GEng()->gui_scale;	// padding
	const int bw = 60 * GEng()->gui_scale;	// buttonwidth
	const int bh = 20 * GEng()->gui_scale;	// buttonheight

	c = new gcn::Window(_(STRING_MENU_MODS));
	c->setDimension(gcn::Rectangle(0, 0, w, h + 15));

	this->modlist = new gcn::DropDown(this->mods);
	this->modlist->setPosition(p, p);
	this->modlist->setWidth(w - p - p);
	this->modlist->setSelected(this->mods->findMod(GEng()->mm->getSupplOrBase()));
	this->modlist->addSelectionListener(this);
	c->add(this->modlist);

	this->icon = new gcn::Icon();
	this->icon->setPosition(p, p + p + this->modlist->getHeight());
	this->icon->setSize(512 * GEng()->gui_scale, 256 * GEng()->gui_scale);
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
	GEng()->mm->remMod(GEng()->mm->getSuppl());

	// If there is a new suppl, add it
	if (newsuppl) {
		GEng()->mm->addMod(newsuppl);
		GEng()->mm->setSuppl(newsuppl);
	}

	this->m->loadModBits();
	this->m->remDialog(this);
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


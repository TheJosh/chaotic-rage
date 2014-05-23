// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include "client_settings.h"

#include <iostream>
#include <math.h>
#include <guichan.hpp>

#include "../rage.h"
#include "../game_state.h"
#include "../game_engine.h"
#include "../render_opengl/menu.h"
#include "../render_opengl/render_opengl.h"
#include "../render_opengl/render_opengl_settings.h"
#include "../render_opengl/guichan_imageloader.h"
#include "../mod/mod.h"
#include "../util/clientconfig.h"
#include "dialog.h"
#include "list_models.h"
#include "../i18n/gettext.h"

#define BUFFER_MAX 100

using namespace std;



/**
* Dialog for client settings
**/
DialogClientSettings::DialogClientSettings(GameState* st) : Dialog()
{
	this->st = st;
}


/**
* Dialog for client settings
**/
DialogClientSettings::~DialogClientSettings()
{
}


/**
* Setup routine for the client settings dialog
**/
gcn::Container * DialogClientSettings::setup()
{
	const int w = 532;	// width
	const int h = 326;	// height
	const int p = 10;	// padding
	const int bw = 200;	// buttonwidth
	const int bh = 20;	// buttonheight
	char buf[BUFFER_MAX];

	gcn::Label* label;

	c = new gcn::Window("Client Settings");
	c->setDimension(gcn::Rectangle(0, 0, w, h + 15));

	RenderOpenGLSettings* gl = ((RenderOpenGL*)GEng()->render)->getSettings();

	label = new gcn::Label("MSAA");
	c->add(label, 10, 10);

	snprintf(buf, BUFFER_MAX, "%i", gl->msaa);
	this->gl_msaa = new gcn::TextField(std::string(buf));
	this->gl_msaa->setPosition(80, 10);
	this->gl_msaa->setWidth(50);
	c->add(this->gl_msaa);

	label = new gcn::Label("Tex Filter");
	c->add(label, 10, 30);

	snprintf(buf, BUFFER_MAX, "%i", gl->tex_filter);
	this->gl_tex_filter = new gcn::TextField(std::string(buf));
	this->gl_tex_filter->setPosition(80, 30);
	this->gl_tex_filter->setWidth(50);
	c->add(this->gl_tex_filter);

	label = new gcn::Label("Language");
	c->add(label, 10, 50);

	this->langs = getAvailableLangs();
	this->lang = new gcn::DropDown(new VectorListModel(this->langs));
	this->lang->setPosition(80, 50);
	this->lang->setWidth(100);
	c->add(this->lang);

	this->button = new gcn::Button("Save");
	this->button->setPosition(w - bw - p, h - bh - p);
	this->button->setSize(bw, bh);
	this->button->addActionListener(this);
	c->add(this->button);
	
	return c;
}


/**
* Clean up the dialog
**/
void DialogClientSettings::tearDown()
{
	c->remove(this->gl_msaa);
	c->remove(this->gl_tex_filter);
	c->remove(this->lang);

	delete this->gl_msaa;
	delete this->gl_tex_filter;
	delete this->lang;
}


/**
* Handle a click on the button
**/
void DialogClientSettings::action(const gcn::ActionEvent& actionEvent)
{
	RenderOpenGLSettings* current = ((RenderOpenGL*)GEng()->render)->getSettings();
	RenderOpenGLSettings* nu = new RenderOpenGLSettings();

	// Populate the class
	nu->msaa = atoi(this->gl_msaa->getText().c_str());
	nu->tex_filter = atoi(this->gl_tex_filter->getText().c_str());
	loadLang(getAvailableLangs()->at(this->lang->getSelected()).c_str());
	this->m->loadModBits();

	// Do we need a restart?
	bool restart = false;
	if (current->msaa != nu->msaa) restart = true;

	// Set
	((RenderOpenGL*)GEng()->render)->setSettings(nu);

	// Save
	GEng()->cconf->gl = nu;
	GEng()->cconf->save();

	// If we need a restart, tell the user
	if (restart) {
		displayMessageBox("One or more of your changes will require a game restart to become effective");
	}

	this->m->remDialog(this);
}


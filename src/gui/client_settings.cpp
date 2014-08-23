// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include "client_settings.h"
#include "../game_engine.h"
#include "../i18n/gettext.h"
#include "../platform/platform.h"
#include "../render_opengl/menu.h"
#include "../render_opengl/render_opengl.h"
#include "../render_opengl/render_opengl_settings.h"
#include "../util/clientconfig.h"
#include "dialog.h"
#include "list_models.h"

class GameState;

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
	const int bw = 200;	// button width
	const int bh = 20;	// button height
	char buf[BUFFER_MAX];

	gcn::Label* label;

	c = new gcn::Window(_(STRING_MENU_SETTINGS));
	c->setDimension(gcn::Rectangle(0, 0, w, h + 15));

	RenderOpenGLSettings* gl = ((RenderOpenGL*)GEng()->render)->getSettings();

	// MSAA
	label = new gcn::Label(_(STRING_SETTINGS_MSAA));
	c->add(label, 10, 10);
	snprintf(buf, BUFFER_MAX, "%i", gl->msaa);
	this->gl_msaa = new gcn::TextField(std::string(buf));
	this->gl_msaa->setPosition(80, 10);
	this->gl_msaa->setWidth(50);
	c->add(this->gl_msaa);

	// Tex filter
	label = new gcn::Label(_(STRING_SETTINGS_TEX_FILTER));
	c->add(label, 10, 30);
	snprintf(buf, BUFFER_MAX, "%i", gl->tex_filter);
	this->gl_tex_filter = new gcn::TextField(std::string(buf));
	this->gl_tex_filter->setPosition(80, 30);
	this->gl_tex_filter->setWidth(50);
	c->add(this->gl_tex_filter);

	// Language
	label = new gcn::Label(_(STRING_SETTINGS_LANGUAGE));
	c->add(label, 10, 50);
	this->langs = getAvailableLangs();
	this->lang = new gcn::DropDown(new VectorListModel(this->langs));
	this->lang->setPosition(80, 50);
	this->lang->setWidth(100);
	c->add(this->lang);
	for (unsigned int i = this->langs->size() - 1; i != 0; --i) {
		if (this->langs->at(i) == GEng()->cconf->lang) {
			this->lang->setSelected(i);
			break;
		}
	}

	// Fullscreen (setting hard-coded if Android)
	#ifndef __ANDROID__
	label = new gcn::Label(_(STRING_SETTINGS_FULLSCREEN));
	c->add(label, 10, 70);
	this->fullscreen = new gcn::CheckBox("", GEng()->cconf->fullscreen);
	this->fullscreen->setPosition(80, 70);
	c->add(this->fullscreen);
	#endif

	// Save button
	this->button = new gcn::Button(_(STRING_SETTINGS_SAVE));
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
	c->remove(this->fullscreen);
	c->remove(this->lang);

	delete this->gl_msaa;
	delete this->gl_tex_filter;
	delete this->fullscreen;
	delete this->lang;
}


/**
* Handle a click on the button
**/
void DialogClientSettings::action(const gcn::ActionEvent& actionEvent)
{
	RenderOpenGLSettings* current = ((RenderOpenGL*)GEng()->render)->getSettings();
	RenderOpenGLSettings* nu = new RenderOpenGLSettings();

	// Fullscreen
	bool currentFS = GEng()->cconf->fullscreen;
	bool newFS = this->fullscreen->isSelected();

	// Populate the class
	nu->msaa = atoi(this->gl_msaa->getText().c_str());
	nu->tex_filter = atoi(this->gl_tex_filter->getText().c_str());

	// Do we need a restart?
	bool restart = false;
	if (current->msaa != nu->msaa || currentFS != newFS) restart = true;

	// Update GL settings
	((RenderOpenGL*)GEng()->render)->setSettings(nu);

	// Re-load language strings
	loadLang(langs->at(this->lang->getSelected()).c_str());
	this->m->loadMenuItems();

	// Save config
	GEng()->cconf->gl = nu;
	GEng()->cconf->fullscreen = newFS;
	GEng()->cconf->lang = langs->at(this->lang->getSelected());
	GEng()->cconf->save();

	// If we need a restart, tell the user
	if (restart) {
		displayMessageBox("A game restart is required to apply all settings");
	}

	this->m->remDialog(this);
}

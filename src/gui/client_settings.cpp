// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include <math.h>
#include <guichan.hpp>
#include <guichan/sdl.hpp>
#include <guichan/opengl.hpp>

#include "../rage.h"
#include "dialog.h"
#include "../render/render_opengl_settings.h"

#include <SDL_image.h>
#include <guichan.hpp>
#include <guichan/sdl.hpp>
#include <guichan/opengl.hpp>
#include "../render/guichan_imageloader.h"


using namespace std;



/**
* Dialog for client settings
**/
DialogClientSettings::DialogClientSettings(GameState* st)
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
	char buf[100];
	
	c = new gcn::Window("Client Settings");
	c->setDimension(gcn::Rectangle(0, 0, w, h + 15));
	
	RenderOpenGLSettings* gl = ((RenderOpenGL*)st->render)->getSettings();
	
	sprintf(buf, "%i", gl->msaa);
	this->gl_msaa = new gcn::TextField(std::string(buf));
	this->gl_msaa->setPosition(80, 10);
	this->gl_msaa->setWidth(50);
	c->add(this->gl_msaa);
	
	sprintf(buf, "%i", gl->tex_filter);
	this->gl_tex_filter = new gcn::TextField(std::string(buf));
	this->gl_tex_filter->setPosition(80, 50);
	this->gl_tex_filter->setWidth(50);
	c->add(this->gl_tex_filter);
	
	this->button = new gcn::Button("Try em out but no saving yet");
	this->button->setPosition(w - bw - p, h - bh - p);
	this->button->setSize(bw, bh);
	this->button->addActionListener(this);
	c->add(this->button);
	
	return c;
}


/**
* Handle a click on the button
**/
void DialogClientSettings::action(const gcn::ActionEvent& actionEvent)
{
	RenderOpenGLSettings* current = ((RenderOpenGL*)st->render)->getSettings();
	RenderOpenGLSettings* nu = new RenderOpenGLSettings();
	
	// Do we need a restart?
	bool restart = false;
	if (current->msaa != nu->msaa) restart = true;
	
	// Populate the class
	nu->msaa = atoi(this->gl_msaa->getText().c_str());
	nu->tex_filter = atoi(this->gl_tex_filter->getText().c_str());
	
	// Set
	((RenderOpenGL*)st->render)->setSettings(nu);
	
	// TODO: If we need a restart, tell the user
	
	this->m->setDialog(NULL);
}


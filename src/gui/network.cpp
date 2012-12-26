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
* Constructor for join network game dialog
**/
DialogNetJoin::DialogNetJoin()
{
}

/**
* Setup routine for the join network game dialog
**/
gcn::Container * DialogNetJoin::setup()
{
	gcn::Button* button;
	
	c = new gcn::Window("Network game");
	c->setDimension(gcn::Rectangle(0, 0, 265, 250));
	
	this->tabs = new gcn::TabbedArea();
	this->tabs->setDimension(gcn::Rectangle(10, 10, 245, 180));
	c->add(this->tabs);
	
	// Direct connect to a host
	{
		gcn::Container* tabc = new gcn::Container();
		tabc->setDimension(gcn::Rectangle(0, 0, 245, 180));
		this->tabs->addTab("Direct", tabc);
		
		gcn::Label* label = new gcn::Label("Host");
		tabc->add(label, 10, 10);
		
		this->host = new gcn::TextField("localhost");
		this->host->setPosition(80, 10);
		this->host->setWidth(150);
		tabc->add(this->host);
	}
	
	// List of internet hosts
	this->internet_hosts = getServerList();
	if (this->internet_hosts != NULL) {
		gcn::Container* tabc = new gcn::Container();
		tabc->setDimension(gcn::Rectangle(0, 0, 245, 180));
		this->tabs->addTab("Internet", tabc);
		
		this->internet = new gcn::ListBox(new VectorListModel(this->internet_hosts));
		this->internet->setPosition(10, 10);
		this->internet->setWidth(225);
		this->internet->setHeight(140);
		tabc->add(this->internet);
	}
	
	button = new gcn::Button("Join Game");
	button->setPosition(192, 200);
	button->addActionListener(this);
	c->add(button);
	
	return c;
}

/**
* Button click processing for the "New Game" dialog
**/
void DialogNetJoin::action(const gcn::ActionEvent& actionEvent)
{
	if (this->tabs->getSelectedTabIndex() == 0) {
		this->m->networkJoin(
			this->host->getText()
		);
		
	} else if (this->tabs->getSelectedTabIndex() == 1) {
		if (this->internet->getSelected() != -1) { 
			this->m->networkJoin(
				this->internet_hosts->at(this->internet->getSelected())
			);
		}
	}
}

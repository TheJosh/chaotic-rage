// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <math.h>
#include <guichan.hpp>
#include <guichan/sdl.hpp>

#include "../rage.h"
#include "../i18n/gettext.h"
#include "../render_opengl/menu.h"
#include "../http/serverlist.h"
#include "../net/net_browse.h"
#include "dialog.h"
#include "list_models.h"
#include "network.h"


using namespace std;



/**
* Constructor for join network game dialog
**/
DialogNetJoin::DialogNetJoin() : Dialog()
{
	this->local_hosts = NULL;
	this->local_model = NULL;
	this->local_refresh_action = new LocalRefreshAction(this);

	this->internet_hosts = NULL;
	this->internet_model = NULL;
	this->internet_refresh_action = new InternetRefreshAction(this);
}


/**
* Destructor for join network game dialog
**/
DialogNetJoin::~DialogNetJoin()
{
	// TODO: kill off widgets

	delete(this->local_hosts);
	delete(this->local_model);
	delete(this->local_refresh_action);

	delete(this->internet_hosts);
	delete(this->internet_model);
	delete(this->internet_refresh_action);
}


/**
* Setup routine for the join network game dialog
**/
gcn::Container * DialogNetJoin::setup()
{
	gcn::Button* button;

	c = new gcn::Window(_(STRING_MENU_NETWORK));
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

	// List of local hosts
	{
		gcn::Container* tabc = new gcn::Container();
		tabc->setDimension(gcn::Rectangle(0, 0, 245, 180));
		this->tabs->addTab("Local", tabc);

		this->local_list = new gcn::ListBox(NULL);
		this->local_list->setPosition(10, 10);
		this->local_list->setWidth(225);
		this->local_list->setHeight(110);
		tabc->add(this->local_list);

		this->local_button = new gcn::Button("Refresh list");
		this->local_button->setPosition(10, 125);
		this->local_button->addActionListener(this->local_refresh_action);
		tabc->add(this->local_button);
	}

	// List of internet hosts
	{
		gcn::Container* tabc = new gcn::Container();
		tabc->setDimension(gcn::Rectangle(0, 0, 245, 180));
		this->tabs->addTab("Internet", tabc);

		this->internet_list = new gcn::ListBox(NULL);
		this->internet_list->setPosition(10, 10);
		this->internet_list->setWidth(225);
		this->internet_list->setHeight(110);
		tabc->add(this->internet_list);

		this->internet_button = new gcn::Button("Refresh list");
		this->internet_button->setPosition(10, 125);
		this->internet_button->addActionListener(this->internet_refresh_action);
		tabc->add(this->internet_button);
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
		if (this->local_list->getSelected() != -1) {
			this->m->networkJoin(
				this->local_hosts->at(this->local_list->getSelected())
			);
		}

	} else if (this->tabs->getSelectedTabIndex() == 2) {
		if (this->internet_list->getSelected() != -1) {
			this->m->networkJoin(
				this->internet_hosts->at(this->internet_list->getSelected())
			);
		}
	}
}


/**
* Local refresh button
**/
void LocalRefreshAction::action(const gcn::ActionEvent& actionEvent)
{
	this->owner->local_list->setListModel(NULL);

	delete(this->owner->local_hosts);
	delete(this->owner->local_model);
	this->owner->local_model = NULL;

	this->owner->local_hosts = getLocalServers(this->owner->m);
	this->owner->local_model = NULL;

	if (this->owner->local_hosts != NULL && this->owner->local_hosts->size() > 0) {
		this->owner->local_model = new VectorListModel(this->owner->local_hosts);
		this->owner->local_list->setListModel(this->owner->local_model);
	}
}


/**
* Internet refresh button
**/
void InternetRefreshAction::action(const gcn::ActionEvent& actionEvent)
{
	this->owner->internet_list->setListModel(NULL);

	delete(this->owner->internet_hosts);
	delete(this->owner->internet_model);

	this->owner->internet_hosts = getServerList(this->owner->m);
	this->owner->internet_model = NULL;

	if (this->owner->internet_hosts != NULL && this->owner->internet_hosts->size() > 0) {
		this->owner->internet_model = new VectorListModel(this->owner->internet_hosts);
		this->owner->internet_list->setListModel(this->owner->internet_model);
	}
}


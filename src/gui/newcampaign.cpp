// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <math.h>
#include <guichan.hpp>
#include <guichan/sdl.hpp>
#include <guichan/opengl.hpp>
#include <guichan/opengl/openglsdlimageloader.hpp>

#include "../rage.h"
#include "../i18n/gettext.h"
#include "../render_opengl/menu.h"
#include "../game_manager.h"
#include "../http/serverlist.h"
#include "../mod/campaign.h"
#include "dialog.h"
#include "list_models.h"
#include "newcampaign.h"


using namespace std;



/**
* List model for campaign dropdown - get element
**/
std::string CampaignListModel::getElementAt(int i)
{
	return campaigns->at(i)->title;
}


/**
* List model for campaign dropdown - num elements
**/
int CampaignListModel::getNumberOfElements()
{
	return campaigns->size();
}


/**
* Constructor for "New Campaign" dialog
**/
DialogNewCampaign::DialogNewCampaign(int num_local, Mod *mod) : Dialog()
{
	this->num_local = num_local;
	this->mod = mod;
	this->campaigns = new CampaignListModel(mod->getCampaigns());
}

DialogNewCampaign::~DialogNewCampaign()
{
	delete(this->campaigns);
}


#define COLLEFT     10
#define COLRIGHT    110
#define COLRIGHTW   195


/**
* Setup routine for the "New Campaign" dialog
**/
gcn::Container * DialogNewCampaign::setup()
{
	gcn::Button* button;
	gcn::Label* label;

	c = new gcn::Window(_(STRING_MENU_CAMPAIGN));
	c->setDimension(gcn::Rectangle(0, 0, 320, 200));


	label = new gcn::Label(_(STRING_MENU_CAMPAIGN));
	c->add(label, COLLEFT, 10);

	this->campaign = new gcn::DropDown(this->campaigns);
	this->campaign->setPosition(COLRIGHT, 10);
	this->campaign->setWidth(COLRIGHTW);
	c->add(this->campaign);


	label = new gcn::Label(_(STRING_NEWGAME_UNIT));
	c->add(label, COLLEFT, 30);

	this->unittype = new gcn::DropDown(new VectorListModel(this->gm->getUnitTypes()));
	this->unittype->setPosition(COLRIGHT, 30);
	this->unittype->setWidth(COLRIGHTW);
	c->add(this->unittype);


	label = new gcn::Label(_(STRING_NEWGAME_VIEW));
	c->add(label, COLLEFT, 50);

	this->viewmode = new gcn::DropDown(new VectorListModel(this->gm->getViewModes()));
	this->viewmode->setPosition(COLRIGHT, 50);
	this->viewmode->setWidth(COLRIGHTW);
	c->add(this->viewmode);


	button = new gcn::Button(_(STRING_NEWGAME_START_CAMPAIGN));
	button->setPosition(120, 150);
	button->addActionListener(this);
	c->add(button);

	return c;
}

/**
* Button click processing for the "New Campaign" dialog
**/
void DialogNewCampaign::action(const gcn::ActionEvent& actionEvent)
{
	this->m->startCampaign(
		this->mod->getCampaigns()->at(this->campaign->getSelected()),
		this->gm->getUnitTypes()->at(this->unittype->getSelected()),
		static_cast<GameSettings::ViewMode>(this->viewmode->getSelected()),
		this->num_local
	);
}

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
#include "../menu.h"
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
DialogNewCampaign::DialogNewCampaign(int num_local, Mod *mod)
{
	this->num_local = num_local;
	this->mod = mod;
	this->campaigns = new CampaignListModel(mod->getCampaigns());
}

DialogNewCampaign::~DialogNewCampaign()
{
	delete(this->campaigns);
}


/**
* Setup routine for the "New Campaign" dialog
**/
gcn::Container * DialogNewCampaign::setup()
{
	gcn::Button* button;
	gcn::Label* label;
	
	c = new gcn::Window("Campaign");
	c->setDimension(gcn::Rectangle(0, 0, 255, 200));
	
	
	label = new gcn::Label("Campaign");
	c->add(label, 10, 10);
	
	this->campaign = new gcn::DropDown(this->campaigns);
	this->campaign->setPosition(80, 10);
	this->campaign->setWidth(160);
	c->add(this->campaign);
	
	
	label = new gcn::Label("Unit type");
	c->add(label, 10, 30);
	
	this->unittype = new gcn::DropDown(new VectorListModel(this->gm->getUnitTypes()));
	this->unittype->setPosition(80, 30);
	this->unittype->setWidth(160);
	c->add(this->unittype);
	
	
	label = new gcn::Label("View mode");
	c->add(label, 10, 50);
	
	this->viewmode = new gcn::DropDown(new VectorListModel(this->gm->getViewModes()));
	this->viewmode->setPosition(80, 50);
	this->viewmode->setWidth(160);
	c->add(this->viewmode);
	
	
	button = new gcn::Button("Start Game");
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
		this->viewmode->getSelected(),
		this->num_local
	);
}


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
#include "../game_engine.h"
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


#define COLLEFT     10 * GEng()->gui_scale
#define COLRIGHT    110 * GEng()->gui_scale
#define COLRIGHTW   195 * GEng()->gui_scale
#define ROWHEIGHT   27 * GEng()->gui_scale


/**
* Setup routine for the "New Campaign" dialog
**/
gcn::Container * DialogNewCampaign::setup()
{
	gcn::Button* button;
	gcn::Label* label;
	int y = 10;

	c = new gcn::Window(_(STRING_MENU_CAMPAIGN));
	c->setDimension(gcn::Rectangle(0, 0, 320 * GEng()->gui_scale, 200 * GEng()->gui_scale));

	label = new gcn::Label(_(STRING_MENU_CAMPAIGN));
	c->add(label, COLLEFT, y);
	this->campaign = new gcn::DropDown(this->campaigns);
	this->campaign->setPosition(COLRIGHT, y);
	this->campaign->setWidth(COLRIGHTW);
	c->add(this->campaign);

	y += ROWHEIGHT;

	label = new gcn::Label(_(STRING_NEWGAME_UNIT));
	c->add(label, COLLEFT, y);
	this->unittype = new gcn::DropDown(new VectorListModel(this->gm->getUnitTypes()));
	this->unittype->setPosition(COLRIGHT, y);
	this->unittype->setWidth(COLRIGHTW);
	c->add(this->unittype);

	y += ROWHEIGHT;

	label = new gcn::Label(_(STRING_NEWGAME_VIEW));
	c->add(label, COLLEFT, y);
	this->viewmode = new gcn::DropDown(new VectorListModel(this->gm->getViewModes()));
	this->viewmode->setPosition(COLRIGHT, y);
	this->viewmode->setWidth(COLRIGHTW);
	c->add(this->viewmode);

	button = new gcn::Button(_(STRING_NEWGAME_START_CAMPAIGN));
	button->setPosition((c->getWidth() - button->getWidth()) / 2, c->getHeight() - button->getHeight() - ROWHEIGHT);
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

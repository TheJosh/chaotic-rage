// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <guichan.hpp>
#include "../rage.h"

using namespace std;



/**
* List model for campaign dropdown
**/
class CampaignListModel: public gcn::ListModel
{
	private:
		vector<Campaign*> * campaigns;
	public:
		CampaignListModel(vector<Campaign*> * campaigns) : campaigns(campaigns) {}
		std::string getElementAt(int i);
		int getNumberOfElements();
};


/**
* Dialog for starting a new campaign - called by the menu
**/
class DialogNewCampaign : public Dialog, public gcn::ActionListener {
	public:
		DialogNewCampaign(int num_local, Mod *mod);
		virtual ~DialogNewCampaign();

	private:
		int num_local;
		Mod* mod;

		gcn::DropDown *campaign;
		CampaignListModel *campaigns;

		gcn::DropDown *unittype;

		gcn::DropDown *viewmode;

	public:
		virtual gcn::Container * setup();
		virtual void action(const gcn::ActionEvent& actionEvent);
};


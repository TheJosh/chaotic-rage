// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <guichan.hpp>
#include "../rage.h"
#include "../http/serverlist.h"

using namespace std;


class LocalRefreshAction;
class InternetRefreshAction;


/**
* Dialog for starting a new game - called by the menu
**/
class DialogNetJoin : public Dialog, public gcn::ActionListener
{
	friend class LocalRefreshAction;
	friend class InternetRefreshAction;

	public:
		DialogNetJoin();
		virtual ~DialogNetJoin();

	private:
		gcn::TabbedArea * tabs;
		gcn::TextField * host;

		gcn::ListBox * local_list;
		gcn::Button * local_button;
		vector<string> * local_hosts;
		VectorListModel * local_model;
		LocalRefreshAction * local_refresh_action;

		gcn::ListBox * internet_list;
		gcn::Button * internet_button;
		vector<string> * internet_hosts;
		VectorListModel * internet_model;
		InternetRefreshAction * internet_refresh_action;

	public:
		virtual gcn::Container * setup();
		virtual void action(const gcn::ActionEvent& actionEvent);
		virtual const DialogName getName() { return NETWORK; }
};


/**
* Handles clicks on the "internet refresh" button
**/
class LocalRefreshAction : public gcn::ActionListener
{
	private:
		DialogNetJoin* owner;

	public:
		explicit LocalRefreshAction(DialogNetJoin* owner) : owner(owner) {}

	public:
		virtual void action(const gcn::ActionEvent& actionEvent);
};


/**
* Handles clicks on the "internet refresh" button
**/
class InternetRefreshAction : public gcn::ActionListener
{
	private:
		DialogNetJoin* owner;

	public:
		explicit InternetRefreshAction(DialogNetJoin* owner) : owner(owner) {}

	public:
		virtual void action(const gcn::ActionEvent& actionEvent);
};

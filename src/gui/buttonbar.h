// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <vector>
#include <guichan.hpp>
#include "dialog.h"


using namespace std;

class GameManager;
class GameState;


/**
* Receives button bar click events
**/
class DialogButtonBarHandler {
	public:
		DialogButtonBarHandler() {}
		virtual ~DialogButtonBarHandler() {}

	public:
		/**
		* This will be called with the index of the button which was pressed
		**/
		virtual void handleButton(int index) = 0;
};


/**
* Shows a bunch of buttons
**/
class DialogButtonBar : public Dialog, public gcn::ActionListener {
	private:
		string title;
		DialogButtonBarHandler *ev;
		vector<gcn::Button*> buttons;

	public:
		DialogButtonBar(string title, vector<string>& labels, DialogButtonBarHandler* ev);
		virtual ~DialogButtonBar();

		virtual gcn::Container* setup();
		virtual void tearDown();

		virtual const string getName() { return "buttonbar"; }
		virtual void action(const gcn::ActionEvent& actionEvent);
};

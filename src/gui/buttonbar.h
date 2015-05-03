// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <string>
#include <vector>
#include <guichan.hpp>
#include "../rage.h"
#include "dialog.h"


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
		std::string title;
		DialogButtonBarHandler *ev;
		std::vector<gcn::Button*> buttons;

	public:
		DialogButtonBar(std::string title, std::vector<std::string>& labels, DialogButtonBarHandler* ev);
		virtual ~DialogButtonBar();

		virtual gcn::Container* setup();
		virtual void tearDown();

		virtual const DialogName getName() { return BUTTONBAR; }
		virtual void action(const gcn::ActionEvent& actionEvent);
};

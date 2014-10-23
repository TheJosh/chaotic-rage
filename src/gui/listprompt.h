// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <vector>
#include <guichan.hpp>
#include "dialog.h"
#include "list_models.h"


using namespace std;

class GameManager;


/**
* Handler base class for list input events
*
* This should be passed as an argument when creating the `DialogListPrompt`.
* Your instance will be delete()ed in the list prompt destructor.
**/
class DialogListPromptHandler {
	public:
		DialogListPromptHandler() {}
		virtual ~DialogListPromptHandler() {}

	public:
		/**
		* This will be called with the string which was entered
		**/
		virtual void handleSelection(int index) = 0;
};


/**
* Allows the user to enter some text
* The `items` and `ev` will be FREED when this dialog is deleted
**/
class DialogListPrompt : public Dialog, public gcn::ActionListener {
	private:
		string title;
		string message;
		vector<string>* items;
		VectorListModel* items_model;
		DialogListPromptHandler *ev;
		gcn::Label* label;
		gcn::DropDown* list;
		gcn::Button* button;

	public:
		DialogListPrompt(string title, string message, vector<string>* items, DialogListPromptHandler* ev);
		virtual ~DialogListPrompt();

		virtual gcn::Container* setup();
		virtual void tearDown();
		virtual const DialogName getName() { return LISTPROMPT; }
		virtual void action(const gcn::ActionEvent& actionEvent);
};


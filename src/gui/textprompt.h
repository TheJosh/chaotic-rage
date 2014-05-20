// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <vector>
#include <guichan.hpp>
#include "dialog.h"


using namespace std;

class GameManager;


/**
* Handler base class for text input events
*
* This should be passed as an argument when creating the `DialogTextPrompt`.
* Your instance will be delete()ed in the text prompt destructor.
**/
class DialogTextPromptHandler {
	public:
		DialogTextPromptHandler() {}
		virtual ~DialogTextPromptHandler() {}

	public:
		/**
		* This will be called with the string which was entered
		**/
		virtual void handleText(string text) = 0;
};


/**
* Allows the user to enter some text
**/
class DialogTextPrompt : public Dialog, public gcn::ActionListener {
	private:
		GameState * st;
		string title;
		string message;
		DialogTextPromptHandler *ev;
		gcn::Label* label;
		gcn::TextField* text;
		gcn::Button* button;

	public:
		DialogTextPrompt(GameState* st, string title, string message, DialogTextPromptHandler* ev);
		virtual ~DialogTextPrompt();

		virtual gcn::Container* setup();
		virtual string getName() { return "textprompt"; }
		virtual void action(const gcn::ActionEvent& actionEvent);
};


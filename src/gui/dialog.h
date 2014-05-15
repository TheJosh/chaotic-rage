// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <vector>
#include "../rage.h"


using namespace std;

class GameManager;
class Menu;


/**
* Base dialog class
**/
class Dialog {
	friend class Menu;
	
	protected:
		gcn::Container* c;
		Menu* m;
		GameManager* gm;
		
	public:
		Dialog() : c(NULL), m(NULL), gm(NULL) {}
		virtual ~Dialog() {}
		
		/**
		* Set up the container, creating all child widgets
		**/
		virtual gcn::Container* setup() = 0;

		/**
		* Destroy child widgets created in the setup() method.
		* Called before the container is removed from the top-level guichan container.
		* Don't remove the actual container though!
		**/
		virtual void tearDown() {}

		/**
		* Return the internal 'name' of the dialog
		**/
		virtual const string getName() const
		{
			return "?";
		}

		/**
		* Return the pointer to the Guichan container object
		**/
		gcn::Container* getContainer();

		/**
		* If this dialog is a currently shown, close it
		**/
		void close();

};


/**
* Dialog complaining about not being implemented yet.
**/
class DialogNull : public Dialog {
	public:
		virtual gcn::Container * setup();
		virtual ~DialogNull() {}
};


/**
* Asks if the user wants to quit.
**/
class DialogQuit : public Dialog, public gcn::ActionListener {
	private:
		GameState * st;
		
	public:
		DialogQuit(GameState * st);
		virtual ~DialogQuit() {}
		
		virtual gcn::Container * setup();
		virtual string getName() { return "quit"; }
		virtual void action(const gcn::ActionEvent& actionEvent);
};


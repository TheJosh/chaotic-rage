// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <vector>
#include <guichan.hpp>


using namespace std;


class GameManager;



/**
* Base dialog class
**/
class Dialog {
	friend class Menu;
	
	protected:
		gcn::Container * c;
		Menu * m;
		GameManager * gm;
		
	public:
		Dialog() : c(NULL), m(NULL), gm(NULL) {}
		virtual ~Dialog() {}
		
		virtual gcn::Container * setup() = 0;
		virtual string getName() { return "?"; }
		gcn::Container * getContainer() { return this->c; };
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


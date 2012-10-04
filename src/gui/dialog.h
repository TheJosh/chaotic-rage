// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include <guichan.hpp>
#include "../rage.h"

using namespace std;



class VectorListModel: public gcn::ListModel
{
	private:
		vector<string> * v;
		
	public:
		VectorListModel(vector<string> * vec) {
			this->v = vec;
		}
		
		std::string getElementAt(int i)
		{
			return v->at(i);
		}
		
		int getNumberOfElements()
		{
			return v->size();
		}
};


class MapRegistryListModel: public gcn::ListModel
{
	private:
		MapRegistry * mapreg;
		
	public:
		MapRegistryListModel(MapRegistry * mapreg) : mapreg(mapreg) {}
		
		std::string getElementAt(int i)
		{
			return mapreg->titleAt(i);
		}
		
		int getNumberOfElements()
		{
			return mapreg->size();
		}
};


/**
* Base dialog class
**/
class Dialog {
	friend class Menu;
	
	protected:
		gcn::Container * c;
		Menu * m;
		
	public:
		virtual gcn::Container * setup() = 0;
		virtual string getName() { return "?"; }
		gcn::Container * getContainer() { return this->c; };
};


/**
* Dialog for starting a new game - called by the menu
**/
class DialogNewGame : public Dialog, public gcn::ActionListener {
	public:
		DialogNewGame(int num_local);
		
	private:
		int num_local;
		gcn::DropDown * map;
		gcn::DropDown * gametype;
		gcn::DropDown * unittype;
		gcn::DropDown * viewmode;
		
	public:
		virtual gcn::Container * setup();
		virtual void action(const gcn::ActionEvent& actionEvent);
};


/**
* Dialog complaining about not being implemented yet.
**/
class DialogNull : public Dialog {
	public:
		virtual gcn::Container * setup();
};


/**
* Dialog displaying in-game controls - called by the menu
**/
class DialogControls : public Dialog {
	public:
		virtual gcn::Container * setup();
};


/**
* Asks if the user wants to quit - in game
**/
class DialogQuit : public Dialog, public gcn::ActionListener {
	private:
		GameState * st;
		
	public:
		DialogQuit(GameState * st);
		virtual gcn::Container * setup();
		virtual string getName() { return "quit"; }
		virtual void action(const gcn::ActionEvent& actionEvent);
};


/**
* Show mod selection
**/
class DialogMods : public Dialog, public gcn::ActionListener {
	public:
		DialogMods();
		virtual gcn::Container * setup();
		virtual string getName() { return "mods"; }
		virtual void action(const gcn::ActionEvent& actionEvent);
};




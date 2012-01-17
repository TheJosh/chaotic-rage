// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include <guichan.hpp>
#include "rage.h"

using namespace std;


class MenuDialog;
class DialogNewGame;
class VectorListModel;


enum MenuCommand {
	MC_NOTHING = 0,
	MC_SINGLEPLAYER = 1,
	MC_SPLITSCREEN = 2,
	MC_NETWORK = 3,
	MC_SETTINGS = 4,
	MC_QUIT = 5,
};

class MenuItem {
	public:
		string name;
		int x1, x2;
		int y1, y2;
		MenuCommand cmd;
		bool hover;
};


class Menu
{
	private:
		GameState *st;
		RenderOpenGL *render;
		int running;
		vector<MenuItem*> menuitems;
		gcn::Gui* gui;
		MenuDialog *dialog;
		

		// This will be removed soon
	public:
		int map;
		vector<string> maps;
		int gametype;
		vector<string> gametypes;
		int viewmode;
		vector<string> viewmodes;
		int unittype;
		vector<string> unittypes;
	
	
	public:
		Menu(GameState *st);
		void doit();
		
	public:
		void menuAdd(string name, int x, int y, MenuCommand cmd);
		void menuRender();
		void menuHover(int x, int y);
		MenuCommand menuClick(int x, int y);

		void setDialog(MenuDialog * dialog);
		void startGame(string map, string gametype, string unittype, int viewmode, int num_local);

	protected:
		void doSingleplayer();
		void doSplitscreen();
		void doNetwork();
		void doSettings();
		void doQuit();
};


class MenuDialog {
	friend class Menu;

	protected:
		gcn::Container * c;
		Menu * m;

	public:
		virtual gcn::Container * setup() = 0;
};


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

class DialogNewGame : public MenuDialog, public gcn::ActionListener {
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

class DialogNull : public MenuDialog {
	public:
		virtual gcn::Container * setup();
};

// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <guichan.hpp>
#include "../rage.h"

using namespace std;


class GametypeListModel;
class VectorListModel;
class GameSettings;
class DialogNewGame_Action_Weapons;
class DialogNewGame_Action_Environment;
class DialogNewGame;
class MapReg;
class MapTile;


/**
* Handler for mouse events for map tiles
**/
class MapTileMouseListener : public gcn::MouseListener
{
	public:
		MapTileMouseListener(MapTile* tile, DialogNewGame* dialog):
			tile(tile),
			dialog(dialog)
			{};

		void setMapTile(MapTile *tile) { this->tile = tile; }

		virtual void mouseClicked(gcn::MouseEvent& mouseEvent);

	protected:
		MapTile* tile;
		DialogNewGame* dialog;
};


/**
* A single "tile" in the scrollable map selection ui
**/
class MapTile
{
	friend class DialogNewGame;
	friend class MapTileMouseListener;

	public:
		MapTile() {};

		MapTile(const MapTile& tile):
			cont(tile.cont),
			icon(tile.icon),
			img(tile.img),
			name(tile.name),
			mouse(tile.mouse),
			reg(tile.reg)
			{
				this->mouse->setMapTile(this);
			};

		~MapTile();

	protected:
		// Internal
		gcn::Container* cont;
		gcn::Icon* icon;
		gcn::Image* img;
		gcn::Label* name;
		MapTileMouseListener* mouse;

		// External
		MapReg *reg;
};


/**
* Dialog for starting a new game - called by the menu
**/
class DialogNewGame : public Dialog, public gcn::ActionListener {
	friend class DialogNewGame_Action_Weapons;
	friend class DialogNewGame_Action_Environment;
	friend class MapTileMouseListener;

	public:
		explicit DialogNewGame(int num_local);
		virtual ~DialogNewGame();

	protected:
		int num_local;
		GameSettings* gs;

	private:
		GametypeListModel *gametype_model;
		VectorListModel *unittype_model;
		VectorListModel *viewmode_model;

		std::vector<MapTile> map_tiles;
		MapTile* map_sel;
		gcn::Image* map_default;

		gcn::DropDown *gametype;
		gcn::DropDown *unittype;
		gcn::DropDown *viewmode;
		gcn::CheckBox *host;

		DialogNewGame_Action_Weapons *action_weapons;
		DialogNewGame_Action_Environment *action_environment;

	public:
		virtual gcn::Container * setup();
		virtual void action(const gcn::ActionEvent& actionEvent);
		virtual const DialogName getName() { return GAME; }

	protected:
		void addAllMapIcons(gcn::Container *c);
		void addMapIcon(gcn::Container *c, int idx, MapReg *reg);
		void selectMapTile(MapTile* tile);
};


/**
* Weapons button
**/
class DialogNewGame_Action_Weapons : public gcn::ActionListener {
	private:
		DialogNewGame *parent;
	public:
		explicit DialogNewGame_Action_Weapons(DialogNewGame *parent): parent(parent) {}
		virtual void action(const gcn::ActionEvent& actionEvent);
};

/**
* Environment button
**/
class DialogNewGame_Action_Environment : public gcn::ActionListener {
	private:
		DialogNewGame *parent;
	public:
		explicit DialogNewGame_Action_Environment(DialogNewGame *parent): parent(parent) {}
		virtual void action(const gcn::ActionEvent& actionEvent);
};

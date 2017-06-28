// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <math.h>
#include <guichan.hpp>
#include <guichan/opengl/openglsdlimageloader.hpp>

#include "../rage.h"
#include "../i18n/gettext.h"
#include "../render_opengl/menu.h"
#include "../render_opengl/guichan_imageloader.h"
#include "../game_manager.h"
#include "../game_settings.h"
#include "../game_engine.h"
#include "../http/serverlist.h"
#include "../mod/mod_manager.h"
#include "dialog.h"
#include "list_models.h"
#include "newgame.h"
#include "newgame_weapons.h"
#include "newgame_environment.h"


using namespace std;


/**
* Constructor for "new game" dialog
**/
DialogNewGame::DialogNewGame(int num_local) : Dialog()
{
	this->num_local = num_local;

	this->map_sel = NULL;

	this->gametype_model = new GametypeListModel(GEng()->mm->getAllGameTypes());

	this->action_weapons = new DialogNewGame_Action_Weapons(this);
	this->action_environment = new DialogNewGame_Action_Environment(this);

	this->gs = new GameSettings();
	
	// TODO: This dialog should allow you you change various game settings
}

DialogNewGame::~DialogNewGame()
{
	delete(this->gametype_model);
	delete(this->unittype_model);
	delete(this->viewmode_model);
	delete(this->action_weapons);
	delete(this->action_environment);
	delete(this->gs);
}


#define COLLEFT     10 * GEng()->gui_scale
#define COLRIGHT    110 * GEng()->gui_scale
#define COLRIGHTW   195 * GEng()->gui_scale
#define ROWHEIGHT   27 * GEng()->gui_scale
#define SMLHEIGHT   23 * GEng()->gui_scale
#define BTNHEIGHT   37 * GEng()->gui_scale


/**
* Setup routine for the "New Game" dialog
**/
gcn::Container * DialogNewGame::setup()
{
	gcn::Button* button;
	gcn::Label* label;
	int y = 10;

	// This is a bit crap. We can't use ->gm until after the dialog is created
	this->unittype_model = new VectorListModel(this->gm->getUnitTypes());
	this->viewmode_model = new VectorListModel(this->gm->getViewModes());


	if (this->num_local == 1) {
		c = new gcn::Window(_(STRING_MENU_SINGLE));
	} else if (this->num_local > 1) {
		c = new gcn::Window(_(STRING_MENU_SPLIT));
	}

	c->setDimension(gcn::Rectangle(0, 0, 800 * GEng()->gui_scale, 550 * GEng()->gui_scale));


	addAllMapIcons(c);
	selectMapTile(&map_tiles.at(0));
	y += 330;


	label = new gcn::Label(_(STRING_NEWGAME_TYPE));
	c->add(label, COLLEFT, y);

	this->gametype = new gcn::DropDown(this->gametype_model);
	this->gametype->setPosition(COLRIGHT, y);
	this->gametype->setWidth(COLRIGHTW);
	c->add(this->gametype);

	button = new gcn::Button(_(STRING_NEWGAME_WEAPONS));
	button->setPosition(COLRIGHT + COLRIGHTW + 20, y - 2);
	button->addActionListener(this->action_weapons);
	c->add(button);

	button = new gcn::Button(_(STRING_NEWGAME_ENVIRONMENT));
	button->setPosition(COLRIGHT + COLRIGHTW + 100, y - 2);
	button->addActionListener(this->action_environment);
	c->add(button);

	y += ROWHEIGHT;

	label = new gcn::Label(_(STRING_NEWGAME_UNIT));
	c->add(label, COLLEFT, y);

	this->unittype = new gcn::DropDown(this->unittype_model);
	this->unittype->setPosition(COLRIGHT, y);
	this->unittype->setWidth(COLRIGHTW);
	c->add(this->unittype);

	y += ROWHEIGHT;

	label = new gcn::Label(_(STRING_NEWGAME_VIEW));
	c->add(label, COLLEFT, y);

	this->viewmode = new gcn::DropDown(this->viewmode_model);
	this->viewmode->setPosition(COLRIGHT, y);
	this->viewmode->setWidth(COLRIGHTW);
	c->add(this->viewmode);

	y += ROWHEIGHT;

	this->host = new gcn::CheckBox(_(STRING_NEWGAME_HOST));
	this->host->setPosition(COLRIGHT, y);
	this->host->setWidth(COLRIGHTW);
	c->add(this->host);

	button = new gcn::Button(this->num_local == 1 ? _(STRING_NEWGAME_START_SINGLE) : _(STRING_NEWGAME_START_SPLIT));
	button->setPosition((c->getWidth() - button->getWidth()) / 2, c->getHeight() - button->getHeight() - ROWHEIGHT);
	button->addActionListener(this);
	c->add(button);

	return c;
}


/**
* Create a scrollarea and map icon for each map, and add to the window
**/
void DialogNewGame::addAllMapIcons(gcn::Container *c)
{
	MapRegistry* maps = this->gm->getMapRegistry();
	gcn::Container* map_cont = new gcn::Container();
	gcn::ScrollArea* map_scroll = new gcn::ScrollArea(map_cont);
	unsigned int num_maps = maps->size();

	gcn::Image::setImageLoader(
		new gcn::ChaoticRageOpenGLSDLImageLoader(GEng()->mm->getBase())
	);
	map_default = gcn::Image::load("default_preview.png", true);

	gcn::Image::setImageLoader(new gcn::OpenGLSDLImageLoader());
	for (unsigned int i = 0; i < num_maps; ++i) {
		MapReg *reg = maps->at(i);
		addMapIcon(map_cont, i, reg);
	}

	map_cont->setSize(5 + num_maps * (256 + 5), 256 + 35);

	map_scroll->setSize(800 - COLLEFT - COLLEFT, 256 + 35 + 12);
	map_scroll->setHorizontalScrollPolicy(gcn::ScrollArea::SHOW_AUTO);
	map_scroll->setVerticalScrollPolicy(gcn::ScrollArea::SHOW_NEVER);
	c->add(map_scroll, 10, 10);
}


/**
* Create a single map icon
**/
void DialogNewGame::addMapIcon(gcn::Container *c, int idx, MapReg *reg)
{
	MapTile tile;
	tile.reg = reg;

	tile.mouse = new MapTileMouseListener(&tile, this);

	tile.cont  = new gcn::Container();
	tile.cont->setPosition(idx * (256 + 5), 0);
	tile.cont->setSize(256 + 10, 256 + 35);
	tile.cont->setOpaque(false);
	tile.cont->setBaseColor(gcn::Color(255, 128, 128));

	tile.icon = new gcn::Icon();
	tile.icon->setPosition(5, 5);
	tile.icon->addMouseListener(tile.mouse);
	tile.cont->add(tile.icon);

	tile.name = new gcn::Label(reg->getTitle());
	tile.name->setPosition(5, 256 + 10);
	tile.name->setAlignment(gcn::Graphics::Alignment::CENTER);
	tile.name->setWidth(256);
	tile.name->addMouseListener(tile.mouse);
	tile.cont->add(tile.name);

	if (reg->getPreview() != "") {
		try {
			tile.img = gcn::Image::load(reg->getPreview());
			tile.icon->setImage(tile.img);
		} catch (gcn::Exception) {
			tile.img = NULL;
			tile.icon->setImage(map_default);
		}
	} else {
		tile.img = NULL;
		tile.icon->setImage(map_default);
	}

	c->add(tile.cont);
	this->map_tiles.push_back(tile);
}


void MapTileMouseListener::mouseClicked(gcn::MouseEvent& mouseEvent)
{
	dialog->selectMapTile(tile);
}


void DialogNewGame::selectMapTile(MapTile* tile)
{
	if (this->map_sel != NULL) {
		map_sel->cont->setOpaque(false);
	}
	tile->cont->setOpaque(true);
	this->map_sel = tile;
}


MapTile::~MapTile()
{
	//delete icon;
	//delete img;
	//delete name;
	//delete mouse;
}


/**
* Button click processing for the "New Game" dialog
**/
void DialogNewGame::action(const gcn::ActionEvent& actionEvent)
{
	this->m->startGame(
		this->map_sel->reg,
		this->gm->getGameTypes()->at(this->gametype->getSelected()),
		this->gm->getUnitTypes()->at(this->unittype->getSelected()),
		static_cast<GameSettings::ViewMode>(this->viewmode->getSelected()),
		this->num_local,
		this->host->isSelected(),
		this->gs
	);
}


/**
* Button click processing for the "weapons" button
**/
void DialogNewGame_Action_Weapons::action(const gcn::ActionEvent& actionEvent)
{
	string gametype = this->parent->gm->getGameTypes()->at(this->parent->gametype->getSelected());

	GameType* gt = GEng()->mm->getGameType(gametype);
	assert(gt);

	this->parent->m->addDialog(new DialogNewGameWeapons(this->parent, this->parent->gs, gt));
}


/**
* Button click processing for the "environment" button
**/
void DialogNewGame_Action_Environment::action(const gcn::ActionEvent& actionEvent)
{
	this->parent->m->addDialog(new DialogNewGameEnvironment(this->parent, this->parent->gs));
}

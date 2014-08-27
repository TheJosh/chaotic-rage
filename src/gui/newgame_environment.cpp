// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <math.h>
#include <guichan.hpp>

#include "../rage.h"
#include "../render_opengl/menu.h"
#include "../game_manager.h"
#include "../game_settings.h"
#include "../game_engine.h"
#include "../mod/mod_manager.h"
#include "../mod/weapontype.h"
#include "../i18n/gettext.h"
#include "dialog.h"
#include "list_models.h"
#include "newgame_environment.h"

#define BUFFER_MAX 10

using namespace std;


/**
* Sub-dialog from the new game dialog for mucking about with weapons
**/
DialogNewGameEnvironment::DialogNewGameEnvironment(DialogNewGame *parent, GameSettings *gs) : Dialog()
{
	this->parent = parent;
	this->gs = gs;
}

DialogNewGameEnvironment::~DialogNewGameEnvironment()
{
	delete(txt_timeofday);
	delete(chk_daynight);
}


#define LEFT        10
#define MIDDLE      150
#define WIDTH       340
#define ROWHEIGHT   25


/**
* Sub-dialog fromtime_of_day the new game dialog for mucking about with weapons
**/
gcn::Container * DialogNewGameEnvironment::setup()
{
	gcn::Button* button;
	gcn::Label* label;
	char buf[BUFFER_MAX];
	int y = 10;

	c = new gcn::Window(_(STRING_NEWGAME_ENVIRONMENT));
	c->setDimension(gcn::Rectangle(0, 0, 360, 300));

	// Time of day
	label = new gcn::Label(_(STRING_ENVIRONMENT_TIMEOFDAY));
	c->add(label, LEFT, y);
	snprintf(buf, BUFFER_MAX, "%.1f", this->gs->time_of_day);
	this->txt_timeofday = new gcn::TextField(std::string(buf));
	this->txt_timeofday->setPosition(MIDDLE, y);
	this->txt_timeofday->setWidth(100);
	c->add(this->txt_timeofday);
	y += ROWHEIGHT;
	
	// Day-night cycle
	chk_daynight = new gcn::CheckBox(_(STRING_ENVIRONMENT_DAY_NIGHT));
	chk_daynight->setPosition(MIDDLE, y);
	chk_daynight->setWidth(WIDTH);
	chk_daynight->setSelected(this->gs->day_night_cycle);
	c->add(chk_daynight);
	y += ROWHEIGHT;

	// Save btn
	y += 5;
	button = new gcn::Button(_(STRING_ENVIRONMENT_SAVE));
	button->setPosition((360 - button->getWidth()) / 2, y);
	button->addActionListener(this);
	c->add(button);
	y += ROWHEIGHT;

	// Make window big enough
	c->setDimension(gcn::Rectangle(0, 0, 360, y + 30));

	return c;
}


/**
* Button click to close and go back to new game dialog
**/
void DialogNewGameEnvironment::action(const gcn::ActionEvent& actionEvent)
{
	this->gs->time_of_day = atof(txt_timeofday->getText().c_str());
	this->gs->day_night_cycle = chk_daynight->isSelected();
	
	this->m->remDialog(this);
}


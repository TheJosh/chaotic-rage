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
	delete(txt_rain);
	delete(txt_snow);
	delete(chk_rndweather);
	delete(chk_gtweather);
}


#define LEFT        10
#define MIDDLE      130
#define WIDTH       340
#define ROWHEIGHT   25
#define SECTHEIGHT  15


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

	y += SECTHEIGHT;

	// Rain
	label = new gcn::Label(_(STRING_ENVIRONMENT_RAIN));
	c->add(label, LEFT, y);
	snprintf(buf, BUFFER_MAX, "%i", this->gs->rain_flow);
	this->txt_rain = new gcn::TextField(std::string(buf));
	this->txt_rain->setPosition(MIDDLE, y);
	this->txt_rain->setWidth(100);
	c->add(this->txt_rain);
	y += ROWHEIGHT;

	// Snow
	label = new gcn::Label(_(STRING_ENVIRONMENT_SNOW));
	c->add(label, LEFT, y);
	snprintf(buf, BUFFER_MAX, "%i", this->gs->snow_flow);
	this->txt_snow = new gcn::TextField(std::string(buf));
	this->txt_snow->setPosition(MIDDLE, y);
	this->txt_snow->setWidth(100);
	c->add(this->txt_snow);
	y += ROWHEIGHT;

	// Random weather
	this->chk_rndweather = new gcn::CheckBox(_(STRING_ENVIRONMENT_RND_WEATHER));
	this->chk_rndweather->setPosition(MIDDLE, y);
	this->chk_rndweather->setWidth(WIDTH);
	this->chk_rndweather->setSelected(this->gs->random_weather);
	c->add(this->chk_rndweather);
	y += ROWHEIGHT;

	// Game type weather
	this->chk_gtweather = new gcn::CheckBox(_(STRING_ENVIRONMENT_GT_WEATHER));
	this->chk_gtweather->setPosition(MIDDLE, y);
	this->chk_gtweather->setWidth(WIDTH);
	this->chk_gtweather->setSelected(this->gs->gametype_weather);
	c->add(this->chk_gtweather);
	y += ROWHEIGHT;

	y += SECTHEIGHT;

	// Save btn
	button = new gcn::Button(_(STRING_ENVIRONMENT_SAVE));
	button->setPosition((360 - button->getWidth()) / 2, y);
	button->addActionListener(this);
	c->add(button);
	y += ROWHEIGHT;

	// Make window big enough
	c->setDimension(gcn::Rectangle(0, 0, 400, y + 30));

	return c;
}


/**
* Button click to close and go back to new game dialog
**/
void DialogNewGameEnvironment::action(const gcn::ActionEvent& actionEvent)
{
	this->gs->time_of_day = atof(this->txt_timeofday->getText().c_str());
	this->gs->day_night_cycle = this->chk_daynight->isSelected();
	this->gs->rain_flow = atoi(this->txt_rain->getText().c_str());
	this->gs->snow_flow = atoi(this->txt_snow->getText().c_str());
	this->gs->random_weather = this->chk_rndweather->isSelected();
	this->gs->gametype_weather = this->chk_gtweather->isSelected();
	
	this->m->remDialog(this);
}


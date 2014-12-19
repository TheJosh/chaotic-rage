// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include "controls.h"
#include <string>
#include "../i18n/gettext.h"
#include "../i18n/strings.h"


/**
* This is the touch implementation of the DialogControls class
* Used for Android
**/


using namespace std;

const static string controls_single[] = {
	"Move", "One finger",
	"Aim", "Two fingers",
	"Fire", "Tap middle of screen",
	//"Melee", "-",
	//"Change Weapon", "-",
	//"Reload", "-",
	//"Use", "-",
	//"Lift/Drop", "-",
	//"Special", "-",
	//"Change Camera", "-",
	//"Play/Pause Music", "-",
	//"Show this help", "-",
	""
};



/**
* Setup routine for the controls dialog
**/
gcn::Container * DialogControls::setup()
{
	int cols[] = {22, 600};


	gcn::Label* label;

	c = new gcn::Window(_(STRING_MENU_HELP));
	c->setDimension(gcn::Rectangle(0, 0, 622, 100));

	int y = 40;
	int i = 0;
	while (!controls_single[i].empty()) {
		label = new gcn::Label(controls_single[i]);
		c->add(label, cols[0], y);

		i++;

		label = new gcn::Label(controls_single[i]);
		label->setAlignment(gcn::Graphics::RIGHT);
		c->add(label, cols[1] - label->getWidth(), y);

		i++;
		y += 17;
	}

	c->setHeight(y + 30);

	return c;
}

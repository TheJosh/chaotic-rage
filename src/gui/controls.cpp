// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <math.h>
#include <guichan.hpp>

#include "../rage.h"
#include "../http/serverlist.h"
#include "dialog.h"
#include "controls.h"


using namespace std;



/**
* Setup routine for the controls dialog
**/
gcn::Container * DialogControls::setup()
{
	string controls_single[] = {
		"Move", "W A S D",
		"Aim", "Mouse",
		"Fire", "Left Click",
		"Melee", "Right Click",
		"Change Weapon", "Scroll",
		"Use", "E",
		"Lift or Drop", "Q",
		"Special", "T",
		"Change Camera", "C",
		""
	};
	
	string controls_split1[] = {
		"Move", "W A S D",
		"Aim", "9 K L P",
		"Fire", "O",
		"Melee", "I",
		"Change Weapon", "J M",
		"Use", "E",
		"Lift or Drop", "Q",
		"Special", "T",
		"Change Camera", "C",
		""
	};
	
	string controls_split2[] = {
		"Move", "Numpad 8 4 5 6",
		"Aim", "Mouse",
		"Fire", "Left Click",
		"Melee", "Right Click",
		"Change Weapon", "Scroll",
		"Use", "Numpad 9",
		"Lift or Drop", "Numpad 7",
		"Special", "Numpad 1",
		"Change Camera", "C",
		""
	};
	
	int cols[] = {22, 200, 222, 400, 422, 600};
	
	
	gcn::Label* label;
	
	c = new gcn::Window("Controls");
	c->setDimension(gcn::Rectangle(0, 0, 622, 100));
	
	label = new gcn::Label("Single Player");
	label->setAlignment(gcn::Graphics::CENTER);
	label->setWidth(178);
	c->add(label, cols[0], 15);
	
	label = new gcn::Label("Split Screen Player 1");
	label->setAlignment(gcn::Graphics::CENTER);
	label->setWidth(178);
	c->add(label, cols[2], 15);
	
	label = new gcn::Label("Split Screen Player 2");
	label->setAlignment(gcn::Graphics::CENTER);
	label->setWidth(178);
	c->add(label, cols[4], 15);
	
	int y = 40;
	int i = 0;
	do {
		if (controls_single[i].length() == 0) break;
		
		label = new gcn::Label(controls_single[i]);
		c->add(label, cols[0], y);
		
		label = new gcn::Label(controls_split1[i]);
		c->add(label, cols[2], y);
		
		label = new gcn::Label(controls_split2[i]);
		c->add(label, cols[4], y);
		
		i++;
		
		label = new gcn::Label(controls_single[i]);
		label->setAlignment(gcn::Graphics::RIGHT);
		c->add(label, cols[1] - label->getWidth(), y);
		
		label = new gcn::Label(controls_split1[i]);
		label->setAlignment(gcn::Graphics::RIGHT);
		c->add(label, cols[3] - label->getWidth(), y);
		
		label = new gcn::Label(controls_split2[i]);
		label->setAlignment(gcn::Graphics::RIGHT);
		c->add(label, cols[5] - label->getWidth(), y);
		
		i++;
		
		y += 17;
	} while(1);
	
	c->setHeight(y + 30);
	
	return c;
}


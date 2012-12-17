// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include <guichan.hpp>
#include "../rage.h"

using namespace std;


/**
* Dialog displaying in-game controls - called by the menu
**/
class DialogControls : public Dialog {
	public:
		virtual gcn::Container * setup();
};

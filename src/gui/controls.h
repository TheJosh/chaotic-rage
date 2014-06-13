// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <guichan.hpp>
#include "../rage.h"

using namespace std;


/**
* Dialog displaying in-game controls
**/
class DialogControls : public Dialog {
	public:
		virtual gcn::Container * setup();
		virtual const DialogName getName() { return CONTROLS; }
		virtual ~DialogControls() {}
};

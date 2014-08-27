// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <guichan.hpp>
#include "../rage.h"

using namespace std;


class DialogNewGame;


/**
* Sub-dialog from the new game dialog for mucking about with weather, etc
**/
class DialogNewGameEnvironment : public Dialog, public gcn::ActionListener {
	public:
		DialogNewGameEnvironment(DialogNewGame* parent, GameSettings* gs);
		virtual ~DialogNewGameEnvironment();

	protected:
		DialogNewGame* parent;
		GameSettings* gs;

		gcn::TextField* txt_timeofday;
		gcn::CheckBox* chk_daynight;

	public:
		virtual gcn::Container * setup();
		virtual void action(const gcn::ActionEvent& actionEvent);
		virtual const DialogName getName() { return ENVIRONMENT; }
};

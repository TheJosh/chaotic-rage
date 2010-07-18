// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


Event::Event()
{
	this->e1 = NULL;
	this->e2 = NULL;
}


/**
* Calls event listeners in the various subsystems
* Frees the provided event object
**/
void fireEvent (Event * ev)
{
	if (ev->e1 != NULL) ev->e1->handleEvent(ev);
	// TODO: should we also call the handleevent on e2?
	
	ev->e1->getGameState()->audio->handleEvent(ev);
	
	delete (ev);
}

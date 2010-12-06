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
	this->a1 = 0;
	this->a2 = 0;
}

/**
* Calls event listeners in the various subsystems
* Frees the provided event object
**/
void fireEvent (Event * ev)
{
	if (ev->e1 != NULL) ev->e1->handleEvent(ev);
	if (ev->e2 != NULL) ev->e2->handleEvent(ev);
	
	getGameState()->audio->handleEvent(ev);
	getGameState()->logic->handleEvent(ev);
	
	// This is for testing only
	//cout << "fireEvent [" << ev->type << "]  e1 = " << ev->e1 << "  e2 = " << ev->e2 << "  a1 = " << ev->a1 << "  a2 = " << ev->a2 << "\n";
	
	delete (ev);
}

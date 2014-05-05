// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <vector>
#include <list>
#include <math.h>
#include <string.h>

#include <guichan.hpp>
#include <guichan/sdl.hpp>

#include "game_engine.h"
#include "render/render.h"
#include "render/render_3d.h"
#include "audio/audio.h"
#include "lua/gamelogic.h"
#include "net/net_server.h"
#include "physics_bullet.h"
#include "util/cmdline.h"
#include "util/clientconfig.h"
#include "util/serverconfig.h"
#include "mod/mod_manager.h"
#include "gui/dialog.h"

using namespace std;


/**
* The global GameEngine object
**/
static GameEngine* g_geng;


/**
* Returns the global GameEngine object
**/
GameEngine* GEng()
{
	return g_geng;
}


/**
* This is a singleton
**/
GameEngine::GameEngine()
{
	this->running = false;
	this->mouse_grabbed = false;
	
	this->render = NULL;
	this->audio = NULL;
	this->server = NULL;
	this->cmdline = NULL;
	this->cconf = NULL;
	this->mm = NULL;

	this->ticksum = 0;
	this->tickindex = 0;
	memset(&this->ticklist, 0, sizeof(this->ticklist));

	g_geng = this;
}


/**
* When this is freed (at the end of the program), it frees all of the subsystems too
**/
GameEngine::~GameEngine()
{
	delete(this->render);
	delete(this->audio);
	delete(this->server);
	delete(this->cmdline);
	delete(this->cconf);
	delete(this->mm);
}


/**
* Sets the status of the mouse grab
* This method is ignored if the debugging option "no-mouse-grab" is set
**/
void GameEngine::setMouseGrab(bool newval)
{
	if (this->cmdline->mouseGrab == false) return;
	
	this->mouse_grabbed = newval;
	this->render->setMouseGrab(newval);
}


/**
* Sets the status of the mouse grab
**/
bool GameEngine::getMouseGrab()
{
	return this->mouse_grabbed;
}


/**
* Init guichan
**/
void GameEngine::initGuichan()
{
#ifdef NOGUI
	this->gui = NULL;
#else
	if (! this->render->is3D()) {
		this->gui = NULL;
		return;
	}
	
	try {
		this->gui = new gcn::Gui();
		this->guiinput = new gcn::SDLInput();
		gui->setInput(guiinput);
		
		((Render3D*)this->render)->initGuichan(gui, this->mm->getDefaultMod());
		
		this->guitop = new gcn::Container();
		this->guitop->setPosition(0,0);
		this->guitop->setSize(this->render->getWidth(), this->render->getHeight());
		this->guitop->setBaseColor(gcn::Color(0, 0, 0, 0));
		gui->setTop(this->guitop);
		
	} catch (const gcn::Exception & ex) {
		this->gui = NULL;
	}
#endif
}


/**
* Is there currently a dialog with the specified name onscreen?
**/
bool GameEngine::hasDialog(string name)
{
	for (list<Dialog*>::iterator it = this->dialogs.begin(); it != this->dialogs.end(); ++it) {
		if ((*it)->getName().compare(name) == 0) return true;
	}
	return false;
}


/**
* Add a dialog to the game world.
* Inits the dialog too.
**/
void GameEngine::addDialog(Dialog * dialog)
{
	if (this->gui == NULL) return;
	
	gcn::Container * c = dialog->setup();
	c->setPosition((this->render->getWidth() - c->getWidth()) / 2, (this->render->getHeight() - c->getHeight()) / 2);
	c->setBaseColor(gcn::Color(150, 150, 150, 200));
	this->guitop->add(c);
	
	this->dialogs.push_back(dialog);
	
	this->setMouseGrab(false);
}


/**
* Remove a dialog from the game world.
**/
void GameEngine::remDialog(Dialog * dialog)
{
	this->dialogs.remove(dialog);
	this->guitop->remove(dialog->getContainer());
	
	if (this->dialogs.empty()) {
		this->setMouseGrab(true);
	}

	delete(dialog);
}


/**
* Are there dialogs currently visible?
**/
bool GameEngine::hasDialogs()
{
	return (!this->dialogs.empty());
}


/**
* Add a new tick to the ringbuffer, for FPS calcs.
* Borrowed from http://stackoverflow.com/questions/87304/calculating-frames-per-second-in-a-game
**/
void GameEngine::calcAverageTick(int newtick)
{
	this->ticksum -= ticklist[tickindex];
	this->ticksum += newtick;
	this->ticklist[tickindex] = newtick;
	
	if(++tickindex == FPS_SAMPLES) {
		this->tickindex = 0;
	}
}


/**
* Return the current average tick length, allowing you to compute the current FPS
**/
float GameEngine::getAveTick()
{
  return ((float)this->ticksum/(float)FPS_SAMPLES);
}



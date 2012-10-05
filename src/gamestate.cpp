// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <string.h>
#include <iostream>
#include <map>
#include <algorithm>
#include <SDL.h>
#include <guichan.hpp>
#include <guichan/sdl.hpp>
#include <math.h>
#include "rage.h"

using namespace std;


static GameState *g_st;


static bool EntityEraser(Entity *e);
static bool ParticleEraser(NewParticle *p);



/**
* Don't use unless if you are in global code
*
* Should this be removed?
**/
GameState * getGameState()
{
	return g_st;
}


/**
* New game state
**/
GameState::GameState()
{
	this->running = false;
	this->anim_frame = 0;
	this->game_time = 0;
	this->num_local = 0;
	this->curr_slot = 1;
	this->reset_mouse = false;
	
	for (unsigned int i = 0; i < MAX_LOCAL; i++) {
		this->local_players[i] = new PlayerState(this);
	}
	
	this->render = NULL;
	this->audio = NULL;
	this->logic = NULL;
	this->client = NULL;
	this->server = NULL;
	
	g_st = this;
}

GameState::~GameState()
{
}


PlayerState::PlayerState(GameState *st)
{
	this->st = st;
	this->p = NULL;
	this->hud = new HUD(this);
	this->slot = 0;
}

PlayerState::~PlayerState()
{
	delete(this->hud);
}


/**
* Add a unit
**/
void GameState::addUnit(Unit* unit)
{
	this->entities_add.push_back(unit);
	this->units.push_back(unit);
	
	if (this->server) this->server->addmsgUnitAdd(unit);
}

/**
* Add a wall
**/
void GameState::addWall(Wall* wall)
{
	this->entities_add.push_back(wall);
	this->walls.push_back(wall);
}

/**
* Add a vehicle
**/
void GameState::addVehicle(Vehicle* vehicle)
{
	this->entities_add.push_back(vehicle);
}

/**
* Add an object
**/
void GameState::addObject(Object* object)
{
	this->entities_add.push_back(object);
}

/**
* Add a particle
**/
void GameState::addNewParticle(NewParticle* particle)
{
	this->particles.push_back(particle);
}


/**
* Finds the unit which matches a given slot
*
* THIS IS CRAP
**/
Unit* GameState::findUnitSlot(int slot)
{
	if (slot <= 0) return NULL;
	
	for (list<Entity*>::iterator it = this->entities.begin(); it != this->entities.end(); it++) {
		if ((*it)->klass() != UNIT) continue;
		if (((Unit*)*it)->slot == slot) return ((Unit*)*it);
	}
	
	return NULL;
}

/**
* Return the PlayerState for a local player, with the given slot number.
* May return NULL (i.e. invalid slot or non-local slot
**/
PlayerState * GameState::localPlayerFromSlot(unsigned int slot)
{
	for (unsigned int i = 0; i < num_local; i++) {
		if (local_players[i]->slot == slot) return local_players[i];
	}
	return NULL;
}

/**
* Used for filtering
**/
static bool EntityEraser(Entity* e)
{
	if (e->del == false) return false;
	delete e;
	return true;
}

/**
* Used for filtering
**/
static bool ParticleEraser(NewParticle* p)
{
	if (p->time_death > g_st->game_time) return false;
	delete p;
	return true;
}


/**
* Gets the entropy for a given player
**/
unsigned int GameState::getEntropy(unsigned int slot)
{
	return this->entropy;
}

/**
* Increases the entropy for a player
**/
void GameState::increaseEntropy(unsigned int slot)
{
	this->entropy += 10;
}



/**
* Clears all game state, ready for a new game
**/
void GameState::clear()
{
	// TODO: Are these leaky?
	this->entities.clear();
	this->entities_add.clear();
	
	// TODO: Are these needed?
	this->units.clear();
	this->walls.clear();
}


/**
* Game has started
**/
void GameState::start()
{
	// It should technically be 0, but 1 avoids division-by-zero
	this->game_time = 1;
	this->anim_frame = 1;
	this->reset_mouse = false;
	
	this->initGuichan();
}


/**
* Updates the state of everything
**/
void GameState::update(int delta)
{
	DEBUG("loop", "Updating gamestate using delta: %i\n", delta);
	
	
	// Add new entities
	this->entities.insert(
		this->entities.end(),
		this->entities_add.begin(),
		this->entities_add.end()
	);
	this->entities_add.clear();
	
	// Update entities
	for (list<Entity*>::iterator it = this->entities.begin(); it != this->entities.end(); it++) {
		Entity *e = (*it);
		if (! e->del) {
			e->update(delta);
		}
	}
	
	// Remove stuff
	this->entities.remove_if(EntityEraser);
	
	// Update physics
	this->physics->stepTime(delta);
	
	// What hit what?
	this->physics->doCollisions();
	
	// Particles
	this->update_particles(delta);
	this->particles.remove_if(ParticleEraser);

	// Decrease entropy
	if (this->entropy > 0) {
		this->entropy--;
	}
	
	// Handle guichan logic
	if (this->hasDialogs()) {
		this->gui->logic();
	}

	// Update time
	this->game_time += delta;
	this->anim_frame = (int) floor(this->game_time * ANIMATION_FPS / 1000.0);
}


/**
* Moves particles
**/
void GameState::update_particles(int delta)
{
	for (list<NewParticle*>::iterator it = this->particles.begin(); it != this->particles.end(); it++) {
		(*it)->pos += (*it)->vel * btScalar(delta);
	}
}


/**
* Finds units visible to this unit.
*
* Returns a UnitQueryResult. Don't forget to delete() it when you are done!
**/
list<UnitQueryResult> * GameState::findVisibleUnits(Unit* origin)
{
	list<UnitQueryResult> * ret = new list<UnitQueryResult>();
	
	float visual_distance = 200;		// TODO: Should this be a unit property?
	
	btTransform trans;
	UnitQueryResult uqr;
	btVector3 vecO, vecS;
	float dist;
	Unit * u;
	
	origin->getRigidBody()->getMotionState()->getWorldTransform(trans);
	vecO = trans.getOrigin();
	
	for (list<Entity*>::iterator it = this->entities.begin(); it != this->entities.end(); it++) {
		if ((*it)->klass() != UNIT) continue;
		u = (Unit*)(*it);
		if (u == origin) continue;

		u->getRigidBody()->getMotionState()->getWorldTransform(trans);
		vecS = trans.getOrigin();
		vecS -= vecO;
		
		dist = vecS.length();
		if (dist > visual_distance) continue;
		
		uqr.u = u;
		uqr.dist = dist;
		
		ret->push_back(uqr);
	}
	
	return ret;
}


/**
* Sets the status of the mouse grab
**/
void GameState::setMouseGrab(bool reset)
{
	this->reset_mouse = reset;
	SDL_ShowCursor(!this->reset_mouse);
	SDL_WM_GrabInput(this->reset_mouse == 1 ? SDL_GRAB_ON : SDL_GRAB_OFF);
}

/**
* Sets the status of the mouse grab
**/
bool GameState::getMouseGrab()
{
	return this->reset_mouse;
}


/**
* Init guichan
**/
void GameState::initGuichan()
{
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
		
	} catch (gcn::Exception ex) {
		this->gui = NULL;
	}
}


bool GameState::hasDialog(string name)
{
	for (list<Dialog*>::iterator it = this->dialogs.begin(); it != this->dialogs.end(); it++) {
		if ((*it)->getName().compare(name) == 0) return true;
	}
	return false;
}


/**
* Add a dialog to the game world.
* Inits the dialog too.
**/
void GameState::addDialog(Dialog * dialog)
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
void GameState::remDialog(Dialog * dialog)
{
	this->dialogs.remove(dialog);
	this->guitop->remove(dialog->getContainer());
	
	if (this->dialogs.size() == 0) {
		this->setMouseGrab(true);
	}

	delete(dialog);
}

/**
* Are there dialogs currently visible?
**/
bool GameState::hasDialogs()
{
	return (this->dialogs.size() != 0);
}


/**
* Send a message to a given slot. Use ALL_SLOTS to send to all slots
**/
void GameState::alertMessage(unsigned int slot, string text)
{
	if (slot == ALL_SLOTS) {
		for (unsigned int i = 0; i < num_local; i++) {
			local_players[i]->hud->addAlertMessage(text);
		}
	} else {
		PlayerState *ps = localPlayerFromSlot(slot);
		if (ps) ps->hud->addAlertMessage(text);
	}
}


/**
* Send a message to a given slot. Use ALL_SLOTS to send to all slots
**/
void GameState::alertMessage(unsigned int slot, string text, string text2)
{
	text.append(text2);
	alertMessage(slot, text);
}



void GameState::addDebugLine(btVector3 * a, btVector3 * b)
{
	DebugLine *dl = new DebugLine();
	dl->a = new btVector3(*a);
	dl->b = new btVector3(*b);
	lines.push_back(dl);
}

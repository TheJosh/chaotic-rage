// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include "game_state.h"

#include <string.h>
#include <iostream>
#include <map>
#include <algorithm>
#include <vector>
#include <list>

#include <guichan.hpp>
#include <guichan/sdl.hpp>
#include <math.h>

#include "rage.h"
#include "game.h"
#include "game_engine.h"
#include "game_settings.h"
#include "lua/gamelogic.h"
#include "map/map.h"
#include "physics_bullet.h"
#include "entity/ammo_round.h"
#include "entity/decaying.h"
#include "entity/entity.h"
#include "entity/object.h"
#include "entity/pickup.h"
#include "entity/unit.h"
#include "entity/vehicle.h"
#include "entity/player.h"
#include "entity/wall.h"
#include "gui/dialog.h"
#include "mod/mod_manager.h"
#include "mod/gametype.h"
#include "render_opengl/hud.h"
#include "render/render_3d.h"
#include "audio/audio.h"
#include "util/cmdline.h"
#include "net/net_client.h"
#include "net/net_server.h"

#ifdef USE_SPARK
#include "spark/SPK.h"
#endif

using namespace std;


static GameState *g_st;


/**
* Called by remove_if to kill off dead entities
* Also reports to clients if it's a server
**/
static bool EntityEraserDead(Entity* e)
{
	if (e->del == false) return false;
	if (GEng()->server != NULL) GEng()->server->addmsgEntityRem(e);
	delete e;
	return true;
}


/**
* Called by remove_if to kill off ALL entities.
* Should only be called at end-of-game
**/
static bool EntityEraserAll(Entity* e)
{
	delete e;
	return true;
}



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
	this->eid_next = 1;

	for (unsigned int i = 0; i < MAX_LOCAL; i++) {
		this->local_players[i] = new PlayerState(this);
	}

	this->logic = NULL;
	this->physics = NULL;
	this->gt = NULL;
	this->gs = NULL;
	this->particle_system = NULL;
	this->particle_renderer = NULL;

	g_st = this;
}

GameState::~GameState()
{
	for (unsigned int i = 0; i < MAX_LOCAL; i++) {
		delete(this->local_players[i]);
	}
}


PlayerState::PlayerState(GameState *st)
{
	this->st = st;
	this->p = NULL;
	this->hud = new HUD(this, reinterpret_cast<RenderOpenGL*>(GEng()->render));
	this->slot = 0;
}

PlayerState::~PlayerState()
{
	delete(this->hud);
}


/**
* Return a unique EID
**/
EID GameState::getNextEID()
{
	return eid_next++;
}


/**
* Return the entity for a given EID
* Is currently O(n), should probably try to make faster
**/
Entity * GameState::getEntity(EID eid)
{
	for (list<Entity*>::iterator it = this->entities.begin(); it != this->entities.end(); ++it) {
		if ((*it)->eid == eid) return (*it);
	}
	for (list<Entity*>::iterator it = this->entities_add.begin(); it != this->entities_add.end(); ++it) {
		if ((*it)->eid == eid) return (*it);
	}
	return NULL;
}


/**
* Add a unit
**/
void GameState::addUnit(Unit* unit)
{
	unit->eid = this->getNextEID();

	this->entities_add.push_back(unit);
	this->units.push_back(unit);
}

/**
* Add a wall
**/
void GameState::addWall(Wall* wall)
{
	wall->eid = this->getNextEID();

	this->entities_add.push_back(wall);
	this->walls.push_back(wall);
}

/**
* Add a vehicle
**/
void GameState::addVehicle(Vehicle* vehicle)
{
	vehicle->eid = this->getNextEID();

	this->entities_add.push_back(vehicle);
}

/**
* Add an object
**/
void GameState::addObject(Object* object)
{
	object->eid = this->getNextEID();

	this->entities_add.push_back(object);
}

/**
* Add a pickup
**/
void GameState::addPickup(Pickup* pickup)
{
	pickup->eid = this->getNextEID();

	this->entities_add.push_back(pickup);

	if (GEng()->server != NULL) {
		GEng()->server->addmsgPickupState(pickup);
	}
}


/**
* Add an AmmoRound
**/
void GameState::addAmmoRound(AmmoRound* ar)
{
	this->entities_add.push_back(ar);

	if (GEng()->server != NULL) {
		GEng()->server->addmsgAmmoRoundState(ar);
	}
}


/**
* It's dead, but not buried!
*
* Marks a given entity as ->del=1
* Creates a new entity in the same location, with the specified animmodel.
* The new entity is of type Decaying
**/
Entity* GameState::deadButNotBuried(Entity* e)
{
	e->del = true;

	Decaying *d = new Decaying(this, e->getTransform(), e->getAnimModel());
	this->entities_add.push_back(d);

	return d;
}


/**
* Finds the unit which matches a given slot
*
* TODO: Make the implementation less crappy and slow.
**/
Unit* GameState::findUnitSlot(unsigned int slot)
{
	for (list<Entity*>::iterator it = this->entities.begin(); it != this->entities.end(); ++it) {
		if ((*it)->klass() != UNIT) continue;
		Unit* u = reinterpret_cast<Unit*>(*it);
		if (u->slot == slot) return u;
	}

	return NULL;
}


/**
* Finds ammoround entities for a given unit.
* Please delete() the result when you are done.
*
* TODO: Make the implementation less crappy and slow.
**/
list<AmmoRound*>* GameState::findAmmoRoundsUnit(Unit* u)
{
	list<AmmoRound*>* out = new list<AmmoRound*>();

	for (list<Entity*>::iterator it = this->entities.begin(); it != this->entities.end(); ++it) {
		if ((*it)->klass() != AMMOROUND) continue;
		if (reinterpret_cast<AmmoRound*>(*it)->owner == u) {
			out->push_back(reinterpret_cast<AmmoRound*>(*it));
		}
	}

	return out;
}


/**
* Return the PlayerState for a local player, with the given slot number.
* May return NULL (i.e. invalid slot or non-local slot)
**/
PlayerState * GameState::localPlayerFromSlot(unsigned int slot)
{
	for (unsigned int i = 0; i < num_local; i++) {
		if (local_players[i]->slot == slot) return local_players[i];
	}
	return NULL;
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
* Add a particle group
* Is a no-op if we don't have the particle system compiled in
**/
void GameState::addParticleGroup(SPK::Group* group)
{
	#ifdef USE_SPARK
		if (this->particle_renderer == NULL) return;
		group->setRenderer(this->particle_renderer);
		this->particle_system->addGroup(group);
	#endif
}


/**
* Game has started
**/
void GameState::preGame()
{
	// It should technically be 0, but 1 avoids division-by-zero
	this->game_time = 1;
	this->anim_frame = 1;

	GEng()->initGuichan();
	GEng()->setMouseGrab(true);

	#ifdef USE_SPARK
		this->particle_system = new SPK::System();
	#endif

	this->entropy = 0;
}


/**
* Clears all game state, ready for a new game
**/
void GameState::postGame()
{
	this->entities.remove_if(EntityEraserAll);
	this->entities_add.remove_if(EntityEraserAll);

	#ifdef USE_SPARK
		delete this->particle_system;
	#endif

	// TODO: Are these needed?
	this->units.clear();
	this->walls.clear();

	this->eid_next = 1;

	GEng()->setMouseGrab(false);
}


// Mouse movement, including keyboard simulation
// TODO: Move these to the PlayerState class
bool ignore_relative_mouse[MAX_LOCAL];
int game_x[MAX_LOCAL], game_y[MAX_LOCAL];
int net_x[MAX_LOCAL], net_y[MAX_LOCAL];
int mk_down_x[MAX_LOCAL], mk_down_y[MAX_LOCAL];


/**
* The main game loop
**/
void GameState::gameLoop(GameState* st, Render* render, Audio* audio, NetClient* client)
{
	int start = 0, net_time = 0, net_timestep = 50;

	for (int i = 0; i < MAX_LOCAL; i++) {
		game_x[i] = game_y[i] = net_x[i] = net_y[i] = mk_down_x[i] = mk_down_y[i] = 0;
		ignore_relative_mouse[i] = false;
	}

	start = SDL_GetTicks();

	this->physics->preGame();
	this->map->preGame();
	render->preGame();
	render->loadHeightmap();

	if (GEng()->server != NULL) {
		GEng()->server->listen();
	}

	if (client == NULL) {
		this->map->loadDefaultEntities();
	} else {
		client->preGame();
	}

	this->preGame();
	this->logic->raise_gamestart();

	if (client == NULL) {
		for (unsigned int i = 0; i < this->num_local; i++) {
			this->logic->raise_playerjoin(this->local_players[i]->slot);
		}
	}

	this->running = true;
	while (this->running) {
		int delta = SDL_GetTicks() - start;
		start = SDL_GetTicks();

		this->logic->update(delta);
		this->update(delta);
		handleEvents(this);

		if (GEng()->getMouseGrab()) {
			if (this->local_players[0]->p) this->local_players[0]->p->angleFromMouse(game_x[0], game_y[0], delta);
			if (this->local_players[1]->p) this->local_players[1]->p->angleFromMouse(game_x[1], game_y[1], delta);
			game_x[0] = game_y[0] = 0;
			game_x[1] = game_y[1] = 0;
		}

		net_time += delta;
		if (net_time > net_timestep) {
			net_time -= net_timestep;

			if (client != NULL) {
				if (this->local_players[0]->p) {
					client->addmsgKeyMouseStatus(net_x[0], net_y[0], net_timestep, this->local_players[0]->p->packKeys());
					net_x[0] = net_y[0] = 0;
				}
				client->update();
			}

			if (GEng()->server != NULL) {
				GEng()->server->update();
			}
		}

		PROFILE_START(render);
		render->render();
		PROFILE_END(render);

		audio->play();
	}

	this->postGame();
	render->postGame();
	render->freeHeightmap();
	audio->postGame();
	this->map->postGame();
	this->physics->postGame();
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
	PROFILE_START(entities);
	for (list<Entity*>::iterator it = this->entities.begin(); it != this->entities.end(); ++it) {
		Entity *e = (*it);
		if (! e->del) {
			e->update(delta);
		}
	}
	PROFILE_END(entities);

	// Remove stuff
	this->entities.remove_if(EntityEraserDead);

	// Update physics
	PROFILE_START(physics);
	this->physics->stepTime(delta);
	PROFILE_END(physics);

	// Particles
	#ifdef USE_SPARK
		this->particle_system->update(delta / 1000.0f);
	#endif

	// Map animationss
	this->map->update(delta);

	// Decrease entropy
	if (this->entropy > 0) {
		this->entropy--;
	}

	// Handle guichan logic
	if (GEng()->hasDialogs()) {
		GEng()->gui->logic();
	}

	// Update FPS stats
	GEng()->calcAverageTick(delta);

	// Update time
	this->game_time += delta;
	this->anim_frame = static_cast<int>(floor(this->game_time * ANIMATION_FPS / 1000.0));
}


/**
* Called by non-gameloop code (e.g. network, scripting) to indicate
* a game-over situation
**/
void GameState::gameOver()
{
	this->running = false;
	this->last_game_result = -1;
}


/**
* Called by non-gameloop code (e.g. network, scripting) to indicate
* a game-over situation
*
* This variant also sets a "result" flag (1 = success, 0 = failure)
* which is passed to the campaign logic to decide what to do next.
**/
void GameState::gameOver(int result)
{
	this->running = false;
	this->last_game_result = result;
}


/**
* Returns the result of the last game.
* A result of 1 indicates success, and 0 indicates failure
* A result of -1 means undefined - network error or likewise
**/
int GameState::getLastGameResult()
{
	return this->last_game_result;
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

	btTransform trans = origin->getTransform();
	UnitQueryResult uqr;
	btVector3 vecO, vecS;
	float dist;
	Unit * u;

	vecO = trans.getOrigin();

	for (list<Entity*>::iterator it = this->entities.begin(); it != this->entities.end(); ++it) {
		if ((*it)->klass() != UNIT) continue;

		u = reinterpret_cast<Unit*>(*it);
		if (u == origin) continue;

		vecS = u->getTransform().getOrigin() - vecO;
		dist = vecS.length();
		if (dist > visual_distance) continue;

		uqr.u = u;
		uqr.dist = dist;

		ret->push_back(uqr);
	}

	return ret;
}


/**
* Returns a set of weapons to give to new units.
**/
vector<WeaponType*>* GameState::getSpawnWeapons(UnitType* ut, Faction fac)
{
	vector<WeaponType*>* ret = new vector<WeaponType*>();

	// Eight weapons ought to be enough for anyone...
	ret->reserve(8);

	// UnitType weapons
	if (gs->factions[fac].spawn_weapons_unit) {
		ret->insert(ret->end(), ut->spawn_weapons.begin(), ut->spawn_weapons.end());
	}

	// GameType weapons
	if (gs->factions[fac].spawn_weapons_gametype) {
		if (gt->factions[fac].spawn_weapons.size()) {
			ret->insert(ret->end(), gt->factions[fac].spawn_weapons.begin(), gt->factions[fac].spawn_weapons.end());
		}
	}

	// GameSettings weapons
	if (gs->factions[fac].spawn_weapons_extra.size()) {
		ret->insert(ret->end(), gs->factions[fac].spawn_weapons_extra.begin(), gs->factions[fac].spawn_weapons_extra.end());
	}

	return ret;
}


/**
* Send a message to a given slot. Use ALL_SLOTS to send to all slots
**/
void GameState::addHUDMessage(unsigned int slot, string text)
{
	if (slot == ALL_SLOTS) {
		for (unsigned int i = 0; i < num_local; i++) {
			local_players[i]->hud->addMessage(text);
		}
	} else {
		PlayerState *ps = localPlayerFromSlot(slot);
		if (ps) ps->hud->addMessage(text);
	}
}


/**
* Send a message to a given slot. Use ALL_SLOTS to send to all slots
**/
void GameState::addHUDMessage(unsigned int slot, string text, string text2)
{
	text.append(text2);
	this->addHUDMessage(slot, text);
}


/**
* Add a label to a given slot. Use ALL_SLOTS to add to all slots
**/
HUDLabel* GameState::addHUDLabel(unsigned int slot, float x, float y, string data)
{

	// This doesn't actually work properly

	PlayerState *ps = this->localPlayerFromSlot(1);

	if (ps && ps->hud) {
		return ps->hud->addLabel(x, y, data);
	}

	return NULL;
}


/**
* Pick for an object using the mouse
* Returns true on hit and false on miss. Sets `hitLocation` and `hitEntity` on success.
*
* TODO: Does this belong here or in the PhysicsBullet class?
**/
bool GameState::mousePick(unsigned int x, unsigned int y, btVector3& hitLocation, Entity** hitEntity)
{
	if (!GEng()->render->is3D()) return false;

	// Get ray coords in world space
	btVector3 start, end;
	reinterpret_cast<Render3D*>(GEng()->render)->mouseRaycast(x, y, start, end);

	this->addDebugLine(&start, &end);

	// Do raycast
	btCollisionWorld::ClosestRayResultCallback cb(start, end);
	cb.m_collisionFilterGroup = CG_UNIT;
	cb.m_collisionFilterMask = this->physics->masks[CG_UNIT];
	this->physics->getWorld()->rayTest(start, end, cb);

	// Hit?
	if (cb.hasHit()) {
		hitLocation = cb.m_hitPointWorld;
		if (cb.m_collisionObject->getUserPointer()) {
			*hitEntity = static_cast<Entity*>(cb.m_collisionObject->getUserPointer());
		} else {
			*hitEntity = NULL;
		}
		return true;
	}

	return false;
}



void GameState::addDebugLine(btVector3 * a, btVector3 * b)
{
	DebugLine *dl = new DebugLine();
	dl->a = new btVector3(*a);
	dl->b = new btVector3(*b);
	lines.push_back(dl);
}

void GameState::addDebugPoint(float x, float y, float z)
{
	this->addDebugPoint(x, y, z, 1.0f);
}

void GameState::addDebugPoint(float x, float y, float z, float len)
{
	DebugLine* dl = new DebugLine();
	dl->a = new btVector3(x - len, y, z);
	dl->b = new btVector3(x + len, y, z);
	lines.push_back(dl);

	dl = new DebugLine();
	dl->a = new btVector3(x, y - len, z);
	dl->b = new btVector3(x, y + len, z);
	lines.push_back(dl);

	dl = new DebugLine();
	dl->a = new btVector3(x, y, z - len);
	dl->b = new btVector3(x, y, z + len);
	lines.push_back(dl);
}


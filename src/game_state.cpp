// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include "game_state.h"

#include <string.h>
#include <vector>
#include <list>

#include <guichan.hpp>
#include <math.h>

#include "rage.h"
#include "events.h"
#include "game_engine.h"
#include "game_settings.h"
#include "script/gamelogic.h"
#include "map/map.h"
#include "physics/physics_bullet.h"
#include "entity/ammo_round.h"
#include "entity/decaying.h"
#include "entity/entity.h"
#include "entity/object.h"
#include "entity/pickup.h"
#include "entity/unit/unit.h"
#include "entity/vehicle.h"
#include "entity/unit/player.h"
#include "entity/wall.h"
#include "mod/gametype.h"
#include "render/render.h"
#include "render/render_3d.h"
#include "render_opengl/hud.h"
#include "render_opengl/animplay.h"
#include "render_opengl/assimpmodel.h"
#include "render_opengl/light.h"
#include "audio/audio.h"
#include "net/net_client.h"
#include "net/net_server.h"
#include "util/cmdline.h"
#include "fx/weather.h"
#include "fx/effects_manager.h"
#include "spark/SPK.h"


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
	this->status = STARTUP;
	this->game_time = 0;
	this->num_local = 0;
	this->eid_next = 1;
	this->time_cycle = 0.0f;
	this->time_of_day = 0.0f;
	this->last_game_result = 0;
	this->map = NULL;
	this->weather = NULL;

	for (unsigned int i = 0; i < MAX_LOCAL; i++) {
		this->local_players[i] = new PlayerState(this);
	}

	this->logic = NULL;
	this->physics = NULL;
	this->gt = NULL;
	this->gs = NULL;
	this->particle_system = NULL;

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
* Add an animation to the renderer
**/
void GameState::addAnimPlay(AnimPlay* play, Entity* e)
{
	GEng()->render->addAnimPlay(play, e);
}


/**
* Remove an animation from the renderer
**/
void GameState::remAnimPlay(AnimPlay* play)
{
	GEng()->render->remAnimPlay(play);
}


/**
* Add a light to the renderer
**/
void GameState::addLight(Light* light)
{
	GEng()->render->addLight(light);
}


/**
* Remove a light from the renderer
**/
void GameState::remLight(Light* light)
{
	GEng()->render->remLight(light);
}


/**
* Set torch state
**/
void GameState::setTorch(bool on)
{
	GEng()->render->setTorch(on);
}


/**
* It's dead, but not buried!
*
* Marks a given entity as ->del=1
* Creates a new entity in the same location, with the specified animmodel.
* The new entity is of type Decaying
**/
Entity* GameState::deadButNotBuried(Entity* e, AnimPlay* play)
{
	e->del = true;

	Decaying *d = new Decaying(this, e->getTransform(), play);
	this->entities_add.push_back(d);

	return d;
}


/**
* Scatter some debris in random directions
**/
void GameState::scatterDebris(Entity* e, unsigned int num, float force, const vector<AssimpModel*>& debris_models)
{
	for (unsigned int i = 0; i <= num; ++i) {
		AssimpModel *model = debris_models.at(getRandom(1, debris_models.size()) - 1);

		AnimPlay play(model);
		Decaying* d = new Decaying(this, e->getTransform(), &play, 1.0f);
		this->entities_add.push_back(d);

		d->body->applyCentralImpulse(btVector3(getRandomf(-force, force), getRandomf(-force, force), getRandomf(-force, force)));
	}
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
* Add a particle group
* Is a no-op if we don't have the particle system compiled in
**/
void GameState::addParticleGroup(SPK::Group* group)
{
	if (this->particle_system == NULL) return;
	this->particle_system->addGroup(group);
}


/**
* Remove a particle group
* Is a no-op if we don't have the particle system compiled in
**/
void GameState::removeParticleGroup(SPK::Group* group)
{
	this->particle_system->removeGroup(group);
}


/**
* Game has started
**/
void GameState::preGame()
{
	// It should technically be 0, but 1 avoids division-by-zero
	this->game_time = 1;

	GEng()->initGuichan();
	GEng()->setMouseGrab(true);

	if (GEng()->render->is3D()) {
		this->particle_system = new SPK::System();
	}

	// Time of day cycle
	this->time_of_day = this->gs->time_of_day;
	this->time_cycle = this->gs->time_of_day < 0.5f ? -this->gs->getTimeOfDayStep() : this->gs->getTimeOfDayStep();
	this->tod_target_val = 0.0f;
	this->tod_target_psec = 0.0f;

	GEng()->render->setAmbient(glm::vec4(this->time_of_day, this->time_of_day, this->time_of_day, 1.0f));
	this->doTorch();

	// Weather
	this->weather = new Weather(this, this->map->width, this->map->height, this->map->weather);
	if (this->gs->rain_flow > 0) this->weather->startRain(this->gs->rain_flow);
	if (this->gs->snow_flow > 0) this->weather->startSnow(this->gs->snow_flow);
	if (this->gs->random_weather) {
		this->weather->enableRandom();
	} else {
		this->weather->disableRandom();
	}
	
	this->effects = new EffectsManager(this);
}


/**
* Clears all game state, ready for a new game
**/
void GameState::postGame()
{
	this->entities.remove_if(EntityEraserAll);
	this->entities_add.remove_if(EntityEraserAll);

	delete this->particle_system;
	delete this->weather;
	delete this->effects;

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
void GameState::gameLoop(Render* render, Audio* audio, NetClient* client)
{
	for (int i = 0; i < MAX_LOCAL; i++) {
		game_x[i] = game_y[i] = net_x[i] = net_y[i] = mk_down_x[i] = mk_down_y[i] = 0;
		ignore_relative_mouse[i] = false;
	}

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
	this->logic->raise_gamestart(this->gs->getRounds());

	if (client == NULL) {
		for (unsigned int i = 0; i < this->num_local; i++) {
			this->logic->raise_playerjoin(this->local_players[i]->slot);
		}
	}

	this->last_tick = SDL_GetTicks();;
	this->status = RUNNING;

	// The main game loop
	while (this->status == RUNNING) {
		this->gameLoopIter();
		if (GEng()->cmdline->throttle) SDL_Delay(1);
		MAINLOOP_ITER
	}

	if (client != NULL) {
		client->addmsgQuit();
		client->update();
	}

	// End screen loop
	while (this->status == END_SCREEN) {
		this->endScreenLoopIter();
		if (GEng()->cmdline->throttle) SDL_Delay(1);
	}

	this->postGame();
	render->postGame();
	render->freeHeightmap();
	audio->postGame();
	this->map->postGame();
	this->physics->postGame();
}


/**
* Internal of the main game loop
**/
void GameState::gameLoopIter()
{
	unsigned int new_tick = SDL_GetTicks();
	float delta = new_tick - this->last_tick;
	this->last_tick = new_tick;

	this->logic->update(delta);
	this->update(delta);
	handleEvents(this);

	if (GEng()->hasDialogs()) {
		GEng()->gui->logic();
	}

	if (GEng()->getMouseGrab()) {
		if (this->local_players[0]->p) this->local_players[0]->p->angleFromMouse(game_x[0], game_y[0], delta);
		if (this->local_players[1]->p) this->local_players[1]->p->angleFromMouse(game_x[1], game_y[1], delta);
		game_x[0] = game_y[0] = 0;
		game_x[1] = game_y[1] = 0;
	}

	if (GEng()->client != NULL) {
		if (this->local_players[0]->p) {
			GEng()->client->addmsgKeyMouseStatus(net_x[0], net_y[0], delta, this->local_players[0]->p->packKeys());
			net_x[0] = net_y[0] = 0;
		}
		GEng()->client->update();
	}

	if (GEng()->server != NULL) {
		if (!GEng()->server->update()) {
			this->status = FINISHED;
		}
	}

	PROFILE_START(render);
	GEng()->render->render();
	PROFILE_END(render);

	GEng()->audio->play();
}


/**
* Post-game loop
**/
void GameState::endScreenLoopIter()
{
	unsigned int new_tick = SDL_GetTicks();
	float delta = new_tick - this->last_tick;
	this->last_tick = new_tick;

	this->logic->update(delta);
	handleEvents(this);

	this->game_time += delta;

	if (GEng()->hasDialogs()) {
		GEng()->gui->logic();
	}

	GEng()->render->render();
	GEng()->audio->play();
}


/**
* Updates the state of everything
**/
void GameState::update(float delta)
{
	DEBUG("loop", "Updating gamestate using delta: %f\n", delta);


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

	// Map
	this->map->update(delta);
	
	// Weather and particles
	if (this->particle_system != NULL) {
		this->weather->update(delta);
		this->particle_system->update(delta / 1000.0f);
	}

	// Time of day cycle
	if (this->tod_target_psec > 0.0f) {
		this->animateTimeOfDay(delta);
		this->doTorch();
	} else if (gs->day_night_cycle) {
		this->doTimeOfDay(delta);
		this->doTorch();
	}

	// Update FPS stats
	GEng()->calcAverageTick(delta);

	// Update time
	this->game_time += delta;
}


/**
* Custom (scripted) day-night animation
**/
void GameState::animateTimeOfDay(float delta)
{
	float change_amt = this->tod_target_psec * delta / 1000.0f;
	float diff = this->time_of_day - this->tod_target_val;

	if (abs(diff) < change_amt) {
		this->time_of_day -= diff;
	} else if (diff < 0.0f) {
		this->time_of_day += change_amt;
	} else {
		this->time_of_day -= change_amt;
	}

	GEng()->render->setAmbient(glm::vec4(this->time_of_day, this->time_of_day, this->time_of_day, 1.0f));
}


/**
* Standard day-night-day-night cycle
**/
void GameState::doTimeOfDay(float delta)
{
	if (this->time_of_day > this->gs->tod_max) {
		this->time_of_day = this->gs->tod_max;
		this->time_cycle = -this->gs->getTimeOfDayStep();
	} else if (this->time_of_day < this->gs->tod_min) {
		this->time_of_day = this->gs->tod_min;
		this->time_cycle = this->gs->getTimeOfDayStep();
	}

	this->time_of_day += this->time_cycle * delta / 1000.0f;

	// Make it darker if it rains
	float rain_flow = this->weather->getRainFlow();
	float ambient = MAX(0.0f, this->time_of_day - 0.5f * rain_flow);
	GEng()->render->setAmbient(glm::vec4(ambient, ambient, ambient, 1.0f));
}


/**
* Recalculate if torch should be on or off
**/
void GameState::doTorch()
{
	if (this->time_of_day < this->gs->tod_max * 0.3f) {
		GEng()->render->setTorch(true);
	} else {
		GEng()->render->setTorch(false);
	}
}


/**
* Called by non-gameloop code (e.g. network, scripting) to indicate
* a game termination - ESC key for example
**/
void GameState::terminate()
{
	this->status = FINISHED;
	this->last_game_result = -1;
}


/**
* Called by non-gameloop code (e.g. network, scripting) to indicate
* a game-over situation
*
* Sets a "result" flag (1 = success, 0 = failure, -1 = undefined - network error or likewise)
* which is passed to the campaign logic to decide what to do next.
**/
void GameState::gameOver(int result)
{
	this->status = END_SCREEN;
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
* Return the PlayerState for a given Player or NULL if not local
**/
PlayerState* GameState::getLocalPlayer(const Player* p)
{
	for (unsigned int i = 0; i < this->num_local; i++) {
		if (local_players[i]->p == p) return local_players[i];
	}
	return NULL;
}


/**
* Return the PlayerState for a given slot or NULL if not local
**/
PlayerState* GameState::getLocalPlayer(unsigned int slot)
{
	for (unsigned int i = 0; i < this->num_local; i++) {
		if (local_players[i]->slot == slot) return local_players[i];
	}
	return NULL;
}


/**
* Send a message to a given slot. Use ALL_SLOTS to send to all slots
**/
void GameState::addHUDMessage(unsigned int slot, string text, string text2)
{
	text.append(text2);
	if (slot == ALL_SLOTS) {
		for (unsigned int i = 0; i < num_local; i++) {
			local_players[i]->hud->addMessage(text);
		}
	} else {
		const PlayerState *ps = this->getLocalPlayer(slot);
		if (ps != NULL) {
			ps->hud->addMessage(text);
		}
	}
}


/**
* Add a label to a given slot. Use ALL_SLOTS to add to all slots
**/
HUDLabel* GameState::addHUDLabel(unsigned int slot, int x, int y, string data, HUDLabel* l)
{
	const PlayerState *ps;

	// TODO: Fix if ALL_SLOTS, HUD label only added to the first slot/player
	if (slot == ALL_SLOTS) {
		for (unsigned int i = 0; i < num_local; i++) {
			ps = local_players[i];
			if (ps && ps->hud) {
				return ps->hud->addLabel(x, y, data, l);
			}
		}
	} else {
		ps = this->getLocalPlayer(slot);
		if (ps != NULL && ps->hud != NULL) {
			return ps->hud->addLabel(x, y, data, l);
		}
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

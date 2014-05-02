// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include "game_manager.h"
#include "game_state.h"
#include "game_engine.h"
#include "game_settings.h"
#include "map.h"
#include "physics_bullet.h"
#include "mod/campaign.h"
#include "net/net.h"
#include "net/net_client.h"
#include "net/net_server.h"
#include "net/net_gameinfo.h"
#include "util/serverconfig.h"
#include "mod/mod_manager.h"
#include "mod/gametype.h"
#include "lua/gamelogic.h"
#include "render/render.h"
#include "audio/audio.h"
#include "util/ui_update.h"

using namespace std;



GameManager::GameManager(GameState *st)
{
	this->st = st;
	this->mapreg = NULL;
}


/**
* Returns a pointer to the loaded map registry
**/
MapRegistry* GameManager::getMapRegistry()
{
	return this->mapreg;
}


/**
* Returns a pointer to the currently loaded game types
**/
vector<string>* GameManager::getGameTypes()
{
	return &(this->gametypes);
}


/**
* Returns a pointer to the currently loaded view modes
**/
vector<string>* GameManager::getViewModes()
{
	return &(this->viewmodes);
}


/**
* Returns a pointer to the currently loaded unit types
**/
vector<string>* GameManager::getUnitTypes()
{
	return &(this->unittypes);
}


/**
* Load stuff from the currently loaded mods into some arrays.
**/
void GameManager::loadModBits(UIUpdate* ui)
{
	Mod* mod = GEng()->mm->getSupplOrBase();

	// Load maps
	delete(this->mapreg);
	this->mapreg = new MapRegistry();
	this->mapreg->find(mod);
	this->mapreg->find("maps");

	if (ui) ui->updateUI();

	// Gametypes
	this->gametypes.clear();
	{
		vector<GameType*> * ut = GEng()->mm->getAllGameTypes();
		for (vector<GameType*>::iterator it = ut->begin(); it != ut->end(); it++) {
			this->gametypes.push_back((*it)->name);
		}
		delete(ut);
	}

	// Viewmodes
	this->viewmodes.clear();
	this->viewmodes.push_back("Behind player");
	this->viewmodes.push_back("Above player");
	this->viewmodes.push_back("First person");
	
	// Unittypes
	this->unittypes.clear();
	{
		vector<UnitType*> * ut = GEng()->mm->getAllUnitTypes();
		for (vector<UnitType*>::iterator it = ut->begin(); it != ut->end(); it++) {
			if ((*it)->playable) this->unittypes.push_back((*it)->name);
		}
		delete(ut);
	}
}


/**
* Plays through each game of a campaign
**/
void GameManager::startCampaign(Campaign* c, string unittype, int viewmode, unsigned int num_local)
{
	for (vector<CampaignStage*>::iterator it = c->stages->begin(); it != c->stages->end();) {
		CampaignStage* stage = *it;

		if (stage->map != "") {
			///-- Game stage --
			MapReg* m = mapreg->get(stage->map);
			if (m == NULL) {
				it++;
				displayMessageBox("Map not found: " + stage->map);
				continue;
			}

			// Run game
			GameSettings* gs = new GameSettings();
			this->startGame(m, stage->gametype, unittype, 0, 1, false, gs);
			delete(gs);

			// A result of 1 is success, 0 is failure, -1 is an error.
			int result = st->getLastGameResult();
			if (result == 1) {
				it++;
			} else if (result == -1) {
				return;			// error
			}
			

		} else if (stage->image_filename != "") {
			///-- Display image --
			/*
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			SpritePtr img = this->render->loadSprite("campaigns/" + stage->image_filename, c->mod);
			if (img) {
				// We don't want the image to wrap
				glBindTexture(GL_TEXTURE_2D, img->pixels);
				// TODO GLES: glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
				// TODO GLES: glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

				// Determine position...
				float width = render->getSpriteWidth(img), height = render->getSpriteHeight(img);
				float ratImg = width/height, ratScr = (float)render->real_width/(float)render->real_height;

				// ...using ratios and fit-with-black-bars system
				if (ratImg > ratScr) {
					width = render->real_width;
					height = render->real_height * ratScr/ratImg;
				} else if (ratImg < ratScr) {
					width = render->real_width * ratImg/ratScr;
					height = render->real_height;
				} else {
					width = render->real_width;
					height = render->real_height;
				}

				// Render image
				render->renderSprite(img, (render->real_width - width) / 2.0f, (render->real_height - height) / 2.0f, width, height);

				// Wait
				unsigned int start = SDL_GetTicks();
				bool running = true;
				SDL_Event event;
				while (running) {
					if ((SDL_GetTicks() - start) > stage->image_time) {
						running = false;
					}

					while (SDL_PollEvent(&event)) {
						if (event.type == SDL_QUIT) {
							this->render->freeSprite(img);
							return;			// window close = campaign exit
						}
					}

					SDL_GL_SwapWindow(((RenderOpenGL*)render)->window);
				}

				this->render->freeSprite(img);
			}
			*/
			it++;
			
		} else {
			assert(0);
		}
	}
}


/**
* Starts a game with the specified settings
*
* @param int viewmode The camera view mode for the local player(s)
* @param int num_local The number of local players; 1 for single, 2+ for splitscreen
* @param int host Set to 1 to host a network game, 0 for a local-only game
**/
void GameManager::startGame(MapReg *map, string gametype, string unittype, int viewmode, unsigned int num_local, bool host, GameSettings *gs)
{
	ServerConfig* sconf = NULL;
	NetServer* server = NULL;
	GameType *gt = NULL;
	Map *m = NULL;
	st->logic = NULL;
	st->map = NULL;
	
	st->physics->init();
	
	if (host) {
		sconf = new ServerConfig();
		server = new NetServer(st, sconf);
		GEng()->server = server;
	}
	
	// Load map
	m = new Map(st);
	if (! m->load(map->getName(), GEng()->render, map->getMod())) {
		displayMessageBox("Failed to load map");
		goto cleanup;
	}
	st->map = m;
	
	// Load gametype
	gt = GEng()->mm->getGameType(gametype);
	assert(gt);
	st->gt = gt;

	// Set game settings
	assert(gs);
	st->gs = gs;

	// Execute lua script
	new GameLogic(st);
	st->logic->selected_unittype = GEng()->mm->getUnitType(unittype);
	st->logic->execScript(gt->script);
	
	// Create players in GameState.
	st->num_local = num_local;
	for (unsigned int i = 0; i < num_local; i++) {
		st->local_players[i] = new PlayerState(st);
		st->local_players[i]->slot = i + 1;
	}

	GEng()->render->viewmode = viewmode;

	// Begin!
	gameLoop(st, GEng()->render, GEng()->audio, NULL);
	
	// Cleanup
cleanup:
	delete st->logic;
	delete st->map;
	st->physics->postGame();
	if (host) {
		GEng()->server = NULL;
		delete(server);
		delete(sconf);
	}
}

	
/**
* Join a network game
**/
void GameManager::networkJoin(string host, UIUpdate *ui)
{
	NetGameinfo *gameinfo;
	MapReg *map;
	Map *m;
	NetClient *client;
	st->logic = NULL;
	st->map = NULL;
	
	client = new NetClient(st);

	st->physics->init();

	// Create local player
	st->num_local = 1;
	st->local_players[0] = new PlayerState(st);

	// Try to join the server
	gameinfo = client->attemptJoinGame(host, 17778, ui);
	if (gameinfo == NULL) {
		displayMessageBox("Unable to connect to server " + host);
		goto cleanup;
	}

	// Try to find the map
	map = this->mapreg->get(gameinfo->map);
	if (! map) {
		displayMessageBox("Map not found: " + gameinfo->map);
		goto cleanup;
	}

	// Load map
	m = new Map(st);
	m->load(map->getName(), GEng()->render, map->getMod());
	st->map = m;
	
	// TODO: Do we need to init the gametype? I think we might
	st->gt = NULL;

	// TODO: Do we need to init the gamesettings? I think we might
	st->gs = NULL;

	// Init GameLogic - is this needed?
	new GameLogic(st);
	st->logic->selected_unittype = GEng()->mm->getUnitType("robot");

	GEng()->render->viewmode = 0;
	
	// Download the gamestate
	// When this is done, a final message is sent to tell the server we are done.
	if (! client->downloadGameState()) {
		displayMessageBox("Unable to download intial game state from server " + host);
		st->postGame();
		goto cleanup;
	}

	// Begin!
	gameLoop(st, GEng()->render, GEng()->audio, client);
	
cleanup:
	delete st->logic;
	delete st->map;
	st->physics->postGame();
	delete client;
}

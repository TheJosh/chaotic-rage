// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include <math.h>
#include "rage.h"
#include "game_state.h"
#include "game_engine.h"
#include "map.h"
#include "physics_bullet.h"
#include "render/render.h"
#include "audio/audio.h"
#include "lua/gamelogic.h"
#include "net/net_client.h"
#include "net/net_server.h"
#include "entity/player.h"
#include "game.h"


using namespace std;


// Mouse movement, including keyboard simulation
int game_x[MAX_LOCAL], game_y[MAX_LOCAL];
int net_x[MAX_LOCAL], net_y[MAX_LOCAL];
int mk_down_x[MAX_LOCAL], mk_down_y[MAX_LOCAL];


/**
* The main game loop
**/
void gameLoop(GameState* st, Render* render, Audio* audio, NetClient* client)
{
	int start = 0, delta = 0, net_time = 0, net_timestep = 50;
	
	for (int i = 0; i < MAX_LOCAL; i++) {
		game_x[i] = game_y[i] = net_x[i] = net_y[i] = mk_down_x[i] = mk_down_y[i] = 0;
	}

	start = SDL_GetTicks();

	st->physics->preGame();
	st->map->preGame();
	render->preGame();
	render->loadHeightmap();

	if (GEng()->server != NULL) {
		GEng()->server->listen();
	}

	if (client == NULL) {
		st->map->loadDefaultEntities();
	} else {
		client->preGame();
	}

	st->preGame();
	st->logic->raise_gamestart();
	
	if (client == NULL) {
		for (unsigned int i = 0; i < st->num_local; i++) {
			st->logic->raise_playerjoin(st->local_players[i]->slot);
		}
	}
	
	st->running = true;
	while (st->running) {
		delta = SDL_GetTicks() - start;
		start = SDL_GetTicks();
		
		st->logic->update(delta);
		st->update(delta);
		handleEvents(st);
		
		if (st->getMouseGrab()) {
			if (st->local_players[0]->p) st->local_players[0]->p->angleFromMouse(game_x[0], game_y[0], delta);
			if (st->local_players[1]->p) st->local_players[1]->p->angleFromMouse(game_x[1], game_y[1], delta);
			game_x[0] = game_y[0] = 0;
			game_x[1] = game_y[1] = 0;
		}
		
		net_time += delta;
		if (net_time > net_timestep) {
			net_time -= net_timestep;
			
			if (client != NULL) {
				if (st->local_players[0]->p) {
					client->addmsgKeyMouseStatus(net_x[0], net_y[0], net_timestep, st->local_players[0]->p->packKeys());
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
	
	st->postGame();
	render->postGame();
	render->freeHeightmap();
	audio->postGame();
	st->map->postGame();
	st->physics->postGame();
}


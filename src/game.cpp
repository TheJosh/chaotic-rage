// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include <math.h>
#include "rage.h"


using namespace std;


// Mouse movement, including keyboard simulation
int game_x[MAX_LOCAL], game_y[MAX_LOCAL];
int net_x[MAX_LOCAL], net_y[MAX_LOCAL];
int mk_down_x[MAX_LOCAL], mk_down_y[MAX_LOCAL];


/**
* The main game loop
**/
void gameLoop(GameState *st, Render *render)
{
	int start = 0, delta = 0, net_time = 0, net_timestep = 50;
	
	for (int i = 0; i < MAX_LOCAL; i++) {
		game_x[i] = game_y[i] = net_x[i] = net_y[i] = mk_down_x[i] = mk_down_y[i] = 0;
	}

	SDL_WM_GrabInput(SDL_GRAB_ON);
	SDL_WarpMouse(400, 300);
	SDL_EnableUNICODE(0);

	start = SDL_GetTicks();

	st->render->preGame();
	st->render->loadHeightmap();

	if (st->client == NULL) {
		st->curr_map->loadDefaultEntities();
	}

	st->start();
	st->setMouseGrab(true);
	st->logic->raise_gamestart();
	
	if (st->client == NULL) {
		for (unsigned int i = 0; i < st->num_local; i++) {
			st->logic->raise_playerjoin(st->local_players[i]->slot);
		}
	}


	cout << "\n\n\n\n\n";
	
	st->running = true;
	while (st->running) {
		delta = SDL_GetTicks() - start;
		start = SDL_GetTicks();
		
		st->logic->update(delta);
		st->update(delta);
		handleEvents(st);
		
		if (st->getMouseGrab()) {
			if (st->local_players[0]->p) st->local_players[0]->p->angleFromMouse(game_x[0], game_y[0], delta);		// one of these two is correct...(this one or the one 15 lines below)
			if (st->local_players[1]->p) st->local_players[1]->p->angleFromMouse(game_x[1], game_y[1], delta);
			game_x[0] = game_y[0] = 0;
			game_x[1] = game_y[1] = 0;
			
			SDL_WarpMouse(400, 300);
		}
		
		net_time += delta;
		if (net_time > net_timestep) {
			net_time -= net_timestep;
			
			if (st->client) {
				if (st->local_players[0]->p && st->getMouseGrab()) {
					//st->local_players[0]->p->angleFromMouse(net_x, net_y, net_timestep);
					st->client->addmsgKeyMouseStatus(net_x[0], net_y[0], net_timestep, st->local_players[0]->p->packKeys());
					net_x[0] = net_y[0] = 0;
				}
				st->client->update();
			}
			
			if (st->server) {
				st->server->update();
			}
		}
		
		st->render->render();
		st->audio->play();
	}
	
	cout << "\n\n\n\n\n";
	
	SDL_WM_GrabInput(SDL_GRAB_OFF);
	SDL_EnableUNICODE(1);

	st->render->postGame();
	st->render->freeHeightmap();
	st->audio->stopAll();
	st->physics->postGame();
	
	st->clear();
}


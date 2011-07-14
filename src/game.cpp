// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include <math.h>
#include "rage.h"

using namespace std;


static void handleEvents(GameState *st);

static bool running;


// Mouse movement
static int game_x, game_y;
static int net_x, net_y;


/**
* The main game loop
**/
void gameLoop(GameState *st, Render *render)
{
	int start = 0, delta = 0, net_time = 0, net_timestep = 50;
	
	game_x = game_y = 0;
	net_x = net_y = 0;
	
	SDL_WM_GrabInput(SDL_GRAB_ON);
	SDL_WarpMouse(400, 30);
	
	st->render->preGame();
	
	st->start();
	st->logic->raise_gamestart();
	
	cout << "\n\n\n\n\n";
	
	running = true;
	while (running) {
		delta = SDL_GetTicks() - start;
		start = SDL_GetTicks();
		
		st->logic->update(delta);
		st->update(delta);
		handleEvents(st);
		
		if (st->curr_player && st->reset_mouse) {
			//st->curr_player->angleFromMouse(game_x, game_y, delta);
			game_x = game_y = 0;
			SDL_WarpMouse(400, 30);
		}
		
		net_time += delta;
		if (net_time > net_timestep) {
			net_time -= net_timestep;
			
			if (st->client) {
				if (st->curr_player && st->reset_mouse) {
					st->curr_player->angleFromMouse(net_x, net_y, net_timestep);
					st->client->addmsgKeyMouseStatus(net_x, net_y, net_timestep, st->curr_player->key);
					net_x = net_y = 0;
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
	
	st->render->postGame();
	
	st->clear();
}


/**
* Handles local events (keyboard, mouse)
**/
static void handleEvents(GameState *st)
{
	SDL_Event event;
	
	while (SDL_PollEvent(&event)) {
		if (st->hud->handleEvent(&event) == HUD::EVENT_PREVENT) continue;
		
		if (event.type == SDL_QUIT) {
			running = false;
			
		} else if (event.type == SDL_KEYDOWN) {
			// Key press
			switch (event.key.keysym.sym) {
				case SDLK_w:
					if (st->curr_player != NULL) st->curr_player->keyPress(Player::KEY_UP);
					break;
					
				case SDLK_a:
					if (st->curr_player != NULL) st->curr_player->keyPress(Player::KEY_LEFT);
					break;
					
				case SDLK_s:
					if (st->curr_player != NULL) st->curr_player->keyPress(Player::KEY_DOWN);
					break;
					
				case SDLK_d:
					if (st->curr_player != NULL) st->curr_player->keyPress(Player::KEY_RIGHT);
					break;
					
				case SDLK_ESCAPE:
					running = false;
					break;
					
					
				case SDLK_PRINT:
					{
						string filename = getUserDataDir();
						filename.append("screenshot");
						((RenderOpenGL*) st->render)->saveScreenshot(filename);
					}
					break;
				
					
				default: break;
			}
			
			
		} else if (event.type == SDL_KEYUP) {
			// Key Release
			switch (event.key.keysym.sym) {
				case SDLK_w:
					if (st->curr_player != NULL) st->curr_player->keyRelease(Player::KEY_UP);
					break;
					
				case SDLK_a:
					if (st->curr_player != NULL) st->curr_player->keyRelease(Player::KEY_LEFT);
					break;
					
				case SDLK_s:
					if (st->curr_player != NULL) st->curr_player->keyRelease(Player::KEY_DOWN);
					break;
					
				case SDLK_d:
					if (st->curr_player != NULL) st->curr_player->keyRelease(Player::KEY_RIGHT);
					break;
					
				case SDLK_ESCAPE:
					running = false;
					break;
					
				default: break;
			}
			
			
		} else if (event.type == SDL_MOUSEMOTION) {
			// Mouse motion
			if (st->curr_player != NULL) {
				cout << "New pos: " << event.motion.x << "\n";

				game_x += event.motion.x - 400;
				net_x += event.motion.x - 400;
				
				game_y += event.motion.y - 30;
				net_y += event.motion.y - 30;
			}
			
		} else if (event.type == SDL_MOUSEBUTTONDOWN) {
			// Mouse down
			if (st->curr_player != NULL) st->curr_player->beginFiring();
			
			
		} else if (event.type == SDL_MOUSEBUTTONUP) {
			// Mouse up
			if (st->curr_player != NULL) st->curr_player->endFiring();
			
			
		}
	}
}



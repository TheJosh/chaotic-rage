// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include <math.h>
#include "rage.h"

using namespace std;


static void handleEvents(GameState *st);


// Mouse movement, including keyboard simulation
static int game_x[MAX_LOCAL], game_y[MAX_LOCAL];
static int net_x[MAX_LOCAL], net_y[MAX_LOCAL];
static int mk_down_x[MAX_LOCAL], mk_down_y[MAX_LOCAL];


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
	SDL_WarpMouse(400, 30);
	
	st->render->preGame();
	st->hud->removeAllDataTables();
	
	st->start();
	
	st->logic->update(1);
	st->logic->raise_gamestart();
	
	if (st->client == NULL) {
		for (unsigned int i = 0; i < st->num_local; i++) {
			st->logic->raise_playerjoin(i+1);		// TODO: Should be based on slot number
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
		
		if (st->reset_mouse) {
			if (st->local_players[0]) st->local_players[0]->angleFromMouse(game_x[0], game_y[0], delta);		// one of these two is correct...(this one or the one 15 lines below)
			if (st->local_players[1]) st->local_players[1]->angleFromMouse(game_x[1], game_y[1], delta);
			game_x[0] = game_y[0] = 0;
			game_x[1] = game_y[1] = 0;
			
			SDL_WarpMouse(400, 30);
		}
		
		net_time += delta;
		if (net_time > net_timestep) {
			net_time -= net_timestep;
			
			if (st->client) {
				if (st->local_players[0] && st->reset_mouse) {
					//st->local_players[0]->angleFromMouse(net_x, net_y, net_timestep);
					st->client->addmsgKeyMouseStatus(net_x[0], net_y[0], net_timestep, st->local_players[0]->packKeys());
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
	
	st->render->postGame();
	st->audio->stopAll();
	st->physics->postGame();
	
	st->clear();
}


/**
* Handles local events (keyboard, mouse)
**/
static void handleEvents(GameState *st)
{
	SDL_Event event;
	
	static int screenshot_num = 0;
	
	
	
	while (SDL_PollEvent(&event)) {
		if (st->hud->handleEvent(&event) == HUD::EVENT_PREVENT) continue;
		
		// General keys
		if (event.type == SDL_QUIT) {
			st->running = false;
			
		} else if (event.type == SDL_KEYDOWN) {
			if (event.key.keysym.sym == SDLK_ESCAPE) {
				st->addDialog(new DialogQuit(st));
				
			} else if (event.key.keysym.sym == SDLK_PRINT) {
				screenshot_num++;
				char buf[50];
				sprintf(buf, "%i", screenshot_num);
				
				string filename = getUserDataDir();
				filename.append("screenshot");
				filename.append(buf);
				filename.append(".bmp");
				((RenderOpenGL*) st->render)->saveScreenshot(filename);
				
				filename = "screenshot";
				filename.append(buf);
				filename.append(".bmp");
				st->hud->addAlertMessage("Saved ", filename);
			}
		}


		// TODO: More dynamic
		
		// One player, player one
		if (st->num_local == 1 && st->local_players[0] != NULL) {
			if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym) {
					case SDLK_w: st->local_players[0]->keyPress(Player::KEY_UP); break;
					case SDLK_a: st->local_players[0]->keyPress(Player::KEY_LEFT); break;
					case SDLK_s: st->local_players[0]->keyPress(Player::KEY_DOWN); break;
					case SDLK_d: st->local_players[0]->keyPress(Player::KEY_RIGHT); break;
					case SDLK_e: st->local_players[0]->keyPress(Player::KEY_USE); break;
					case SDLK_q: st->local_players[0]->keyPress(Player::KEY_LIFT); break;
					case SDLK_t: st->local_players[0]->keyPress(Player::KEY_SPECIAL); break;
					
					
					case SDLK_KP7: ((RenderOpenGL*)st->render)->tx++; break;
					case SDLK_KP8: ((RenderOpenGL*)st->render)->ty++; break;
					case SDLK_KP9: ((RenderOpenGL*)st->render)->tz++; break;
					case SDLK_KP4: ((RenderOpenGL*)st->render)->tx--; break;
					case SDLK_KP5: ((RenderOpenGL*)st->render)->ty--; break;
					case SDLK_KP6: ((RenderOpenGL*)st->render)->tz--; break;
					
					case SDLK_KP1: ((RenderOpenGL*)st->render)->rx++; break;
					case SDLK_KP2: ((RenderOpenGL*)st->render)->rx--; break;
					
					
					case SDLK_F5:
						st->hud->addAlertMessage("Config reload: ", st->mm->reloadAttrs() ? "success" : "failure");
						break;
						
					case SDLK_F6:
						st->reset_mouse = !st->reset_mouse;
						SDL_ShowCursor(!st->reset_mouse);
						SDL_WM_GrabInput(st->reset_mouse == 1 ? SDL_GRAB_ON : SDL_GRAB_OFF);
						st->hud->addAlertMessage("Reset-mouse ", st->reset_mouse ? "on" : "off");
						break;
						
					default: break;
				}
			
			} else if (event.type == SDL_KEYUP) {
				switch (event.key.keysym.sym) {
					case SDLK_w: st->local_players[0]->keyRelease(Player::KEY_UP); break;
					case SDLK_a: st->local_players[0]->keyRelease(Player::KEY_LEFT); break;
					case SDLK_s: st->local_players[0]->keyRelease(Player::KEY_DOWN); break;
					case SDLK_d: st->local_players[0]->keyRelease(Player::KEY_RIGHT); break;
					case SDLK_e: st->local_players[0]->keyRelease(Player::KEY_USE); break;
					case SDLK_q: st->local_players[0]->keyRelease(Player::KEY_LIFT); break;
					case SDLK_t: st->local_players[0]->keyRelease(Player::KEY_SPECIAL); break;
					default: break;
				}
			
			} else if (event.type == SDL_MOUSEMOTION) {
				game_x[0] += event.motion.x - 400;
				net_x[0] += event.motion.x - 400;
				game_y[0] += event.motion.y - 30;
				net_y[0] += event.motion.y - 30;
			
			} else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == 1) {
				st->local_players[0]->keyPress(Player::KEY_FIRE);
			
			} else if (event.type == SDL_MOUSEBUTTONUP && event.button.button == 1) {
				st->local_players[0]->keyRelease(Player::KEY_FIRE);

			} else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == 3) {
				st->local_players[0]->keyPress(Player::KEY_MELEE);
			
			} else if (event.type == SDL_MOUSEBUTTONUP && event.button.button == 3) {
				st->local_players[0]->keyRelease(Player::KEY_MELEE);

			}

		} // end one player, one


		// Two players, player one
		if (st->num_local == 2 && st->local_players[0] != NULL) {
			if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym) {
					case SDLK_w: st->local_players[0]->keyPress(Player::KEY_UP); break;
					case SDLK_a: st->local_players[0]->keyPress(Player::KEY_LEFT); break;
					case SDLK_s: st->local_players[0]->keyPress(Player::KEY_DOWN); break;
					case SDLK_d: st->local_players[0]->keyPress(Player::KEY_RIGHT); break;
					case SDLK_e: st->local_players[0]->keyPress(Player::KEY_USE); break;
					case SDLK_q: st->local_players[0]->keyPress(Player::KEY_LIFT); break;
					case SDLK_o: mk_down_x[0] = -10; break;
					case SDLK_p: mk_down_x[0] = 10; break;
					case SDLK_9: mk_down_y[0] = -10; break;
					case SDLK_l: mk_down_y[0] = 10; break;
					case SDLK_k: st->local_players[0]->keyPress(Player::KEY_FIRE); break;
					default: break;
				}
			
			} else if (event.type == SDL_KEYUP) {
				switch (event.key.keysym.sym) {
					case SDLK_w: st->local_players[0]->keyRelease(Player::KEY_UP); break;
					case SDLK_a: st->local_players[0]->keyRelease(Player::KEY_LEFT); break;
					case SDLK_s: st->local_players[0]->keyRelease(Player::KEY_DOWN); break;
					case SDLK_d: st->local_players[0]->keyRelease(Player::KEY_RIGHT); break;
					case SDLK_e: st->local_players[0]->keyRelease(Player::KEY_USE); break;
					case SDLK_q: st->local_players[0]->keyRelease(Player::KEY_LIFT); break;
					case SDLK_o: mk_down_x[0] = 0; break;
					case SDLK_p: mk_down_x[0] = 0; break;
					case SDLK_9: mk_down_y[0] = 0; break;
					case SDLK_l: mk_down_y[0] = 0; break;
					case SDLK_k: st->local_players[0]->keyRelease(Player::KEY_FIRE); break;
					default: break;
				}
			}

		} // Two players, player one


		// Two players, player two
		// Keypad: Move = 8, 4, 5, 6; Action = 9; Lift = 7
		if (st->num_local == 2 && st->local_players[1] != NULL) {
			if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym) {
					case SDLK_KP8: st->local_players[1]->keyPress(Player::KEY_UP); break;
					case SDLK_KP4: st->local_players[1]->keyPress(Player::KEY_LEFT); break;
					case SDLK_KP5: st->local_players[1]->keyPress(Player::KEY_DOWN); break;
					case SDLK_KP6: st->local_players[1]->keyPress(Player::KEY_RIGHT); break;
					case SDLK_KP9: st->local_players[1]->keyPress(Player::KEY_USE); break;
					case SDLK_KP7: st->local_players[1]->keyPress(Player::KEY_LIFT); break;
					default: break;
				}
			
			} else if (event.type == SDL_KEYUP) {
				switch (event.key.keysym.sym) {
					case SDLK_KP8: st->local_players[1]->keyRelease(Player::KEY_UP); break;
					case SDLK_KP4: st->local_players[1]->keyRelease(Player::KEY_LEFT); break;
					case SDLK_KP5: st->local_players[1]->keyRelease(Player::KEY_DOWN); break;
					case SDLK_KP6: st->local_players[1]->keyRelease(Player::KEY_RIGHT); break;
					case SDLK_KP9: st->local_players[1]->keyRelease(Player::KEY_USE); break;
					case SDLK_KP7: st->local_players[1]->keyRelease(Player::KEY_LIFT); break;
					default: break;
				}
			
			} else if (event.type == SDL_MOUSEMOTION) {
				game_x[1] += event.motion.x - 400;
				net_x[1] += event.motion.x - 400;
				game_y[1] += event.motion.y - 30;
				net_y[1] += event.motion.y - 30;
			
			} else if (event.type == SDL_MOUSEBUTTONDOWN) {
				st->local_players[1]->keyPress(Player::KEY_FIRE);
			
			} else if (event.type == SDL_MOUSEBUTTONUP) {
				st->local_players[1]->keyRelease(Player::KEY_FIRE);
			}

		} // Two players, player two


	} // end while


	// Process simulated mouse movements
	if (st->num_local == 2 && st->local_players[0] != NULL) {
		game_x[0] += mk_down_x[0];
		net_x[0] += mk_down_x[0];
		game_y[0] += mk_down_y[0];
		net_y[0] += mk_down_y[0];
	}
}



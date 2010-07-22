// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


static void handleEvents(GameState *st);

static bool running;
static int fps;


/**
* The main game loop
**/
void gameLoop(GameState *st, Render *render)
{
	int start = 0, end = 1, delta = 0;
	int curr_frame = 0, total_time = 0;
	
	SDL_WarpMouse(400, 30);
	
	running = true;
	while (running) {
		delta = end - start;
		start = SDL_GetTicks();
		
		curr_frame++;
		total_time += delta;
		fps = round(((float) curr_frame) / ((float) total_time / 1000.0));
		
		st->update(delta);
		handleEvents(st);
		
		st->render->render();
		st->audio->play();
		
		SDL_WarpMouse(400, 30);
		SDL_Delay(2);
		
		end = SDL_GetTicks();
	}
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
					st->curr_player->keyPress(Player::KEY_UP);
					break;
					
				case SDLK_a:
					st->curr_player->keyPress(Player::KEY_LEFT);
					break;
					
				case SDLK_s:
					st->curr_player->keyPress(Player::KEY_DOWN);
					break;
					
				case SDLK_d:
					st->curr_player->keyPress(Player::KEY_RIGHT);
					break;
					
				case SDLK_ESCAPE:
					running = false;
					break;
					
				default: break;
			}
			
			
		} else if (event.type == SDL_KEYUP) {
			// Key Release
			switch (event.key.keysym.sym) {
				case SDLK_w:
					st->curr_player->keyRelease(Player::KEY_UP);
					break;
					
				case SDLK_a:
					st->curr_player->keyRelease(Player::KEY_LEFT);
					break;
					
				case SDLK_s:
					st->curr_player->keyRelease(Player::KEY_DOWN);
					break;
					
				case SDLK_d:
					st->curr_player->keyRelease(Player::KEY_RIGHT);
					break;
					
				case SDLK_ESCAPE:
					running = false;
					break;
					
				default: break;
			}
			
			
		} else if (event.type == SDL_MOUSEMOTION) {
			// Mouse motion
			st->curr_player->angleFromMouse(event.motion.x, event.motion.y);
			
			
		} else if (event.type == SDL_MOUSEBUTTONDOWN) {
			// Mouse down
			st->curr_player->beginFiring();
			
			
		} else if (event.type == SDL_MOUSEBUTTONUP) {
			// Mouse up
			st->curr_player->endFiring();
			
			
		}
	}
}



#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


static void updateState(GameState *st, int ms_delta);
static void handleEvents(GameState *st);

static bool running;


/**
* The main game loop
**/
void gameLoop(GameState *st, SDL_Surface *screen)
{
	int start = 1, end = 0;
	
	running = true;
	while (running) {
		updateState(st, end - start);
		
		start = SDL_GetTicks();
		
		handleEvents(st);
		render(st, screen);
		SDL_Delay(10);
		
		end = SDL_GetTicks();
	}
}

/**
* Updates the state of everything
**/
static void updateState(GameState *st, int delta)
{
	int i;
	
	DEBUG("Updating gamestate using delta: %i\n", delta);
	
	for (i = 0; i < st->numUnits(); i++) {
		st->getUnit(i)->update(delta);
	}
}


/**
* Handles local events (keyboard, mouse)
**/
static void handleEvents(GameState *st)
{
	SDL_Event event;
	
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) {
			running = false;
			
		} else if (event.type == SDL_KEYDOWN) {
			// Key press
			switch (event.key.keysym.sym) {
				case SDLK_w:
					st->curr_player->keyPress(Player::KEY_FWD);
					break;
					
				case SDLK_s:
					st->curr_player->keyPress(Player::KEY_REV);
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
					st->curr_player->keyRelease(Player::KEY_FWD);
					break;
					
				case SDLK_s:
					st->curr_player->keyRelease(Player::KEY_REV);
					break;
					
				case SDLK_ESCAPE:
					running = false;
					break;
					
				default: break;
			}
			
			
		} else if (event.type == SDL_MOUSEMOTION) {
			st->curr_player->angleFromMouse(event.motion.x, event.motion.y);
			
		}
	}
}

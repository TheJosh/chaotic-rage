#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


static void updateState(GameState *st, int ms_delta);
static void handleEvents(GameState *st);

static bool running;
static int delay = 50;


void gameLoop(GameState *st, SDL_Surface *screen)
{
	int start = 0, end = 0;
	
	running = true;
	while (running) {
		updateState(st, (end - start) * 1000);
		
		start = SDL_GetTicks();
		
		handleEvents(st);
		render(st, screen);
		SDL_Delay(delay);
		
		end = SDL_GetTicks();
	}
}

static void updateState(GameState *st, int usdelta)
{
	int i;
	
	DEBUG("Updating gamestate using delta: %i\n", usdelta);
	
	for (i = 0; i < st->numUnits(); i++) {
		st->getUnit(i)->update(usdelta);
	}
}

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

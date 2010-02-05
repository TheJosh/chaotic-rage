#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


static void updateState(GameState *st, int ms_delta);
static void handleEvents(GameState *st);
static void renderState(GameState *st);

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
			
		} else if (event.type == SDL_KEYUP) {
			if (event.key.keysym.sym == SDLK_j) {
				delay--;
				printf("delay: %i\n", delay);
			}
			
			if (event.key.keysym.sym == SDLK_k) {
				delay++;
				printf("delay: %i\n", delay);
			}
			
			if (delay < 1) delay = 1;
		}
	}
}

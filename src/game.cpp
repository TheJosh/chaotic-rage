#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


static void updateState(GameState *st, int ms_delta);
static void handleEvents(GameState *st);
static void renderState(GameState *st);

static bool running;


void gameLoop(GameState *st, SDL_Surface *screen)
{
	int start = 0, end = 0;
	
	running = true;
	while (running) {
		updateState(st, end - start);
		
		start = SDL_GetTicks();
		
		handleEvents(st);
		render(st, screen);
		SDL_Delay(1);
		
		end = SDL_GetTicks();
	}
}

static void updateState(GameState *st, int ms_delta)
{
	int i;
	
	for (i = 0; i < st->numUnits(); i++) {
		st->getUnit(i)->update(ms_delta);
	}
}

static void handleEvents(GameState *st)
{
	SDL_Event event;
	
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) {
			running = false;
		}
	}
}

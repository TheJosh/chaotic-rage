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
void gameLoop(GameState *st, SDL_Surface *screen)
{
	int start = 0, end = 1, delta = 0;
	int curr_frame = 0, total_time = 0;
	
	running = true;
	while (running) {
		delta = end - start;
		start = SDL_GetTicks();
		
		curr_frame++;
		total_time += delta;
		fps = round(((float) curr_frame) / ((float) total_time / 1000.0));
		
		st->update(delta);
		handleEvents(st);
		render(st, screen);
		
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



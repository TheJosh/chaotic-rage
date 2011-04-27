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
static float fps;


/**
* The main game loop
**/
void gameLoop(GameState *st, Render *render)
{
	int start = 0, delta = 0;
	int curr_frame = 0, total_time = 0;
	Event * ev;
	
	SDL_WM_GrabInput(SDL_GRAB_ON);
	SDL_WarpMouse(400, 30);
	
	st->render->preGame();
	
	ev = new Event();
	ev->type = GAME_STARTED;
	fireEvent(ev);
	
	running = true;
	while (running) {
		delta = SDL_GetTicks() - start;
		start = SDL_GetTicks();
		
		curr_frame++;
		total_time += delta;
		fps = round(((float) curr_frame) / ((float) total_time / 1000.0));
		
		st->logic->update(delta);
		st->update(delta);
		handleEvents(st);
		if (st->reset_mouse) SDL_WarpMouse(400, 30);
		
		st->render->render();
		st->audio->play();
		
		if (delta < 5) SDL_Delay(10);	// give up some CPU time if we have some to spare
	}
	
	ev = new Event();
	ev->type = GAME_ENDED;
	fireEvent(ev);
	
	SDL_WM_GrabInput(SDL_GRAB_OFF);
	
	st->render->postGame();
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
					
					
				case SDLK_x:
					((RenderOpenGL*) st->render)->x++;
					break;
					
				case SDLK_c:
					((RenderOpenGL*) st->render)->x--;
					break;
					
				case SDLK_y:
					((RenderOpenGL*) st->render)->y++;
					break;
					
				case SDLK_u:
					((RenderOpenGL*) st->render)->y--;
					break;
					
				case SDLK_z:
					((RenderOpenGL*) st->render)->z++;
					break;
					
				case SDLK_b:
					((RenderOpenGL*) st->render)->z--;
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
			if (st->curr_player != NULL) st->curr_player->angleFromMouse(event.motion.x, event.motion.y);
			
			
		} else if (event.type == SDL_MOUSEBUTTONDOWN) {
			// Mouse down
			if (st->curr_player != NULL) st->curr_player->beginFiring();
			
			
		} else if (event.type == SDL_MOUSEBUTTONUP) {
			// Mouse up
			if (st->curr_player != NULL) st->curr_player->endFiring();
			
			
		}
	}
}



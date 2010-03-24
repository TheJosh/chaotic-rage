#include <iostream>
#include <SDL.h>
#include "rage.h"


using namespace std;



HUD::HUD()
{
	this->weapon_menu = false;
}


/**
* Render the heads up display
**/
void HUD::render(SDL_Surface *screen)
{
	if (this->weapon_menu) {
		SDL_Rect r = {0, 0, 50, 50};
		SDL_FillRect(screen, &r, 0);
	}
}


/**
* Handles events.
* Called before the main event processing occurs.
* Return EVENT_BUBBLE to bubble the event, EVENT_PREVENT to prevent additional processing
**/
int HUD::handleEvent(SDL_Event *event)
{
	if (event->type == SDL_KEYUP and event->key.keysym.sym == SDLK_LSHIFT) {
		st->hud->weapon_menu = ! st->hud->weapon_menu;
		return HUD::EVENT_PREVENT;
		
	} else if (this->weapon_menu and event->type == SDL_MOUSEBUTTONDOWN) {
		this->weapon_menu = false;
		return HUD::EVENT_BUBBLE;
		
	}
	
	return HUD::EVENT_BUBBLE;
}

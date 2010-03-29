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
		SDL_Rect r = {100, 100, 125, 125};
		unsigned int i, num = getNumWeaponTypes();
		
		for (i = 0; i < num; i++) {
			WeaponType *wt = getWeaponTypeByID(i);
			
			SDL_BlitSurface(wt->icon_large, NULL, screen, &r);
			
			r.x += 150;
			if (r.x >= 600) {
				r.x = 100;
				r.y += 150;
			}
		}
	}
}

/**
* Changes the weapon if it can
* X + Y co-ordinates in the weapon menu
**/
void HUD::tryChangeWeapon(int x, int y)
{
	SDL_Rect r = {100, 100, 125, 125};
	unsigned int i, num = getNumWeaponTypes();
	
	for (i = 0; i < num; i++) {
		if (inside(r, x, y)) {
			WeaponType *wt = getWeaponTypeByID(i);
			this->st->curr_player->setWeapon(wt);
			return;
		}
		
		r.x += 150;
		if (r.x >= 600) {
			r.x = 100;
			r.y += 150;
		}
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
		// SHIFT key
		st->hud->weapon_menu = ! st->hud->weapon_menu;
		return HUD::EVENT_PREVENT;
		
	} else if (this->weapon_menu and event->type == SDL_MOUSEBUTTONDOWN) {
		// Mouse click
		this->tryChangeWeapon(event->button.x, event->button.y);
		
		this->weapon_menu = false;
		return HUD::EVENT_BUBBLE;
		
	}
	
	return HUD::EVENT_BUBBLE;
}

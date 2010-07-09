// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

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
void HUD::render(Render * render)
{
	if (this->weapon_menu) {
		SDL_Rect r = {100, 100, 125, 125};
		unsigned int i, num = this->st->curr_player->getNumWeapons();
		
		for (i = 0; i < num; i++) {
			WeaponType *wt = this->st->curr_player->getWeaponAt(i);
			
			render->renderSprite(wt->icon_large, r.x, r.y);
			
			if (i == this->st->curr_player->getCurrentWeaponID()) {
				render->renderSprite(wt->icon_large, 500, 500);
			}
			
			r.x += 150;
			if (r.x >= 600) {
				r.x = 100;
				r.y += 150;
			}
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
		// MOUSE BUTTON + SCROLL
		if (event->button.button == SDL_BUTTON_WHEELUP) {
			this->st->curr_player->setWeapon(this->st->curr_player->getPrevWeaponID());
			return HUD::EVENT_PREVENT;
			
		} else if (event->button.button == SDL_BUTTON_WHEELDOWN) {
			this->st->curr_player->setWeapon(this->st->curr_player->getNextWeaponID());
			return HUD::EVENT_PREVENT;
			
		} else {
			this->weapon_menu = false;
			return HUD::EVENT_BUBBLE;
		}
	}
	
	return HUD::EVENT_BUBBLE;
}

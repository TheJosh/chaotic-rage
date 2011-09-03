// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include "rage.h"


using namespace std;



HUD::HUD(GameState *st)
{
	st->hud = this;
	this->st = st;
	
	this->weapon_menu = false;
	this->spawn_menu = false;
}


void HUD::showSpawnMenu()
{
	spawn_menu = true;
	this->st->reset_mouse = false;
	SDL_ShowCursor(SDL_ENABLE);
}

void HUD::hideSpawnMenu()
{
	spawn_menu = false;
	
	this->st->reset_mouse = true;
	SDL_ShowCursor(SDL_DISABLE);
}

void HUD::addAlertMessage(string text)
{
	AlertMessage *msg = new AlertMessage();
	msg->text = text;
	msg->remove_time = this->st->game_time + 5000;
	this->msgs.push_front(msg);
}

void HUD::addAlertMessage(string text1, string text2)
{
	AlertMessage *msg = new AlertMessage();
	msg->text = text1.append(text2);
	msg->remove_time = this->st->game_time + 5000;
	this->msgs.push_front(msg);
}


/**
* Used for filtering
**/
/*static bool MessageEraser(AlertMessage *e)
{
	return (e->remove_time > st->game_time);
}*/


/**
* Render the heads up display
**/
void HUD::render(RenderOpenGL * render)
{
	if (this->spawn_menu) {
		// TODO: menu for spawning
		for (int i = 0; i <= 1; i++) {
			UnitType *uc = st->getDefaultMod()->getUnitType(i);
			UnitTypeState *ucs = uc->getState(UNIT_STATE_STATIC);
			if (! uc->playable) continue;
			
			glPushMatrix();
			glTranslatef(100 + i * 100, 500, 0);
			glRotatef(90, 1, 0, 0);
			glRotatef(180, 0, 0, 1);
			
			AnimPlay * play = new AnimPlay(ucs->model);
			((RenderOpenGL*) render)->renderAnimPlay(play);
			delete(play);
			
			glPopMatrix();
		}
		
		
	} else if (this->weapon_menu && this->st->local_players[0]) {
		// Weapon menu
		SDL_Rect r = {100, 100, 125, 125};
		unsigned int i, num = this->st->local_players[0]->getNumWeapons();
		
		for (i = 0; i < num; i++) {
			WeaponType *wt = this->st->local_players[0]->getWeaponAt(i);
			
			render->renderSprite(wt->icon_large, r.x, r.y);
			
			if (i == this->st->local_players[0]->getCurrentWeaponID()) {
				render->renderSprite(wt->icon_large, 500, 500);
			}
			
			r.x += 150;
			if (r.x >= 600) {
				r.x = 100;
				r.y += 150;
			}
		}
		
		
	} else {
		// General gameplay stuff
		int y = 1000;
		
		for (list<AlertMessage*>::iterator it = this->msgs.begin(); it != this->msgs.end(); it++) {
			AlertMessage *msg = (*it);
			
			if (msg->remove_time < st->game_time) {
				// TODO: Remove the message
				continue;
			}
			
			y -= 20;
			render->renderText(msg->text, 20, y);
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
	if (event->type == SDL_KEYUP) {
		if (event->key.keysym.sym == SDLK_LSHIFT) {
			// SHIFT key
			st->hud->weapon_menu = ! st->hud->weapon_menu;
			return HUD::EVENT_PREVENT;

		} else if (event->key.keysym.sym == SDLK_UP) {
			this->st->local_players[0]->setWeapon(this->st->local_players[0]->getPrevWeaponID());
			return HUD::EVENT_PREVENT;
			
		}  else if (event->key.keysym.sym == SDLK_DOWN) {
			this->st->local_players[0]->setWeapon(this->st->local_players[0]->getNextWeaponID());
			return HUD::EVENT_PREVENT;
			
		}

	} else if (this->weapon_menu and event->type == SDL_MOUSEBUTTONDOWN) {
		// MOUSE BUTTON + SCROLL
		if (event->button.button == SDL_BUTTON_WHEELUP) {
			this->st->local_players[0]->setWeapon(this->st->local_players[0]->getPrevWeaponID());
			return HUD::EVENT_PREVENT;
			
		} else if (event->button.button == SDL_BUTTON_WHEELDOWN) {
			this->st->local_players[0]->setWeapon(this->st->local_players[0]->getNextWeaponID());
			return HUD::EVENT_PREVENT;
			
		} else {
			this->weapon_menu = false;
			return HUD::EVENT_BUBBLE;
		}
	}
	
	return HUD::EVENT_BUBBLE;
}

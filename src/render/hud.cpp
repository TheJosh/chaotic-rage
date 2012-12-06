// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include "../rage.h"


using namespace std;



HUD::HUD(PlayerState *ps)
{
	this->ps = ps;
	this->weapon_menu = false;
}


void HUD::addMessage(string text)
{
	HUDMessage *msg = new HUDMessage();
	msg->text = text;
	msg->remove_time = this->ps->st->game_time + 5000;
	this->msgs.push_front(msg);
}

void HUD::addMessage(string text1, string text2)
{
	HUDMessage *msg = new HUDMessage();
	msg->text = text1.append(text2);
	msg->remove_time = this->ps->st->game_time + 5000;
	this->msgs.push_front(msg);
}


/**
* Add a data table to the HUD
**/
HUDLabel * HUD::addLabel(int x, int y, string data)
{
	HUDLabel * l = new HUDLabel(x, y, data);
	l->width = ((Render3D*)this->ps->st->render)->virt_width;

	this->labels.push_back(l);
	return l;
}


/**
* Used for filtering
**/
/*static bool MessageEraser(HUDMessage *e)
{
	return (e->remove_time > st->game_time);
}*/


/**
* Render the heads up display
**/
void HUD::render(Render3D * render)
{
	if (this->weapon_menu && this->ps->p) {
		// Weapon menu
		SDL_Rect r = {100, 100, 125, 125};
		unsigned int i, num = this->ps->p->getNumWeapons();
		
		for (i = 0; i < num; i++) {
			WeaponType *wt = this->ps->p->getWeaponTypeAt(i);
			
			render->renderText(wt->title, r.x, r.y);

			if (i == this->ps->p->getCurrentWeaponID()) {
				render->renderText(">", r.x - 25, r.y);
			}
			
			r.y += 30;
		}
		
		
	} else {
		// Messages
		int y = 1000;
		for (list<HUDMessage*>::iterator it = this->msgs.begin(); it != this->msgs.end(); it++) {
			HUDMessage *msg = (*it);
			
			if (msg->remove_time < ps->st->game_time) {
				// TODO: Remove the message
				continue;
			}
			
			y -= 20;
			render->renderText(msg->text, 20, y);
		}
		
		// Labels
		for (list<HUDLabel*>::iterator it = this->labels.begin(); it != this->labels.end(); it++) {
			HUDLabel *l = (*it);
			if (! l->visible) continue;
			
			if (l->align == HUDLabelAlign::ALIGN_LEFT) {
				render->renderText(l->data, l->x, l->y);

			} else if (l->align == HUDLabelAlign::ALIGN_CENTER) {
				int w = render->widthText(l->data);
				render->renderText(l->data, l->x + (l->width - w) / 2, l->y);

			} else if (l->align == HUDLabelAlign::ALIGN_RIGHT) {
				int w = render->widthText(l->data);
				render->renderText(l->data, l->x + (l->width - w), l->y);
			}
		}
		
		// Health, ammo, etc
		if (this->ps->p != NULL) {
			int val;
			
			val = this->ps->p->getMagazine();
			if (val >= 0) {
				char buf[50];
				sprintf(buf, "%i", val);
				render->renderText(buf, 50, 50);
			} else if (val == -2) {
				render->renderText("relodn!", 50, 50);
			}
			
			val = this->ps->p->getBelt();
			if (val >= 0) {
				char buf[50];
				sprintf(buf, "%i", val);
				render->renderText(buf, 150, 50);
			}
			
			val = this->ps->p->getHealth();
			if (val >= 0) {
				char buf[50];
				sprintf(buf, "%i", val);
				render->renderText(buf, 50, 80);
			}
		}
	}
}


/**
* Up scroll or equivelent
**/
void HUD::eventUp()
{
	this->weapon_menu = true;
	this->ps->p->setWeapon(this->ps->p->getPrevWeaponID());
}


/**
* Down scroll or equivelent
**/
void HUD::eventDown()
{
	this->weapon_menu = true;
	this->ps->p->setWeapon(this->ps->p->getNextWeaponID());
}


/**
* Click or equivelent
**/
void HUD::eventClick()
{
	this->weapon_menu = false;
}

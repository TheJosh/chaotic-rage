// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include "../rage.h"
#include "../game_engine.h"
#include "../game_state.h"
#include "../entity/player.h"
#include "../mod/weapontype.h"
#include "../net/net_server.h"
#include "hud.h"
#include "render_opengl.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#define BUFFER_MAX 50

using namespace std;



HUD::HUD(PlayerState *ps, RenderOpenGL *render)
{
	this->ps = ps;
	this->render = render;
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
HUDLabel * HUD::addLabel(float x, float y, string data)
{
	HUDLabel * l = new HUDLabel(x, y, data);
	l->width = (float) (this->render)->virt_width;
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
void HUD::draw()
{
	if (this->weapon_menu && this->ps->p) {
		// Weapon menu
		float x = 100;
		float y = 100;
		unsigned int i;
		unsigned int num = this->ps->p->getNumWeapons();

		for (i = 0; i < num; i++) {
			WeaponType *wt = this->ps->p->getWeaponTypeAt(i);

			this->render->renderText(wt->title, x, y);

			if (i == this->ps->p->getCurrentWeaponID()) {
				this->render->renderText(">", x - 25.0f, y);
			}

			y += 30.0f;
		}


	} else {
		// Messages
		float y = (float) (this->render)->virt_height;
		for (list<HUDMessage*>::iterator it = this->msgs.begin(); it != this->msgs.end(); ++it) {
			HUDMessage *msg = (*it);

			if (msg->remove_time < ps->st->game_time) {
				// TODO: Remove the message
				continue;
			}

			y -= 20.0f;
			this->render->renderText(msg->text, 20.0f, y);
		}

		// Labels
		for (list<HUDLabel*>::iterator it = this->labels.begin(); it != this->labels.end(); ++it) {
			HUDLabel *l = (*it);
			// Send state over network
			if (GEng()->server != NULL) {
				GEng()->server->addmsgHUD(l);
			}
			if (! l->visible) continue;

			if (l->align == ALIGN_LEFT) {
				this->render->renderText(l->data, l->x, l->y, l->r, l->g, l->b, l->a);

			} else if (l->align == ALIGN_CENTER) {
				int w = render->widthText(l->data);
				this->render->renderText(l->data, l->x + (l->width - w) / 2, l->y, l->r, l->g, l->b, l->a);

			} else if (l->align == ALIGN_RIGHT) {
				int w = render->widthText(l->data);
				this->render->renderText(l->data, l->x + (l->width - w), l->y, l->r, l->g, l->b, l->a);
			}
		}

		// Health, ammo, etc
		if (this->ps->p != NULL) {
			int val;

			val = this->ps->p->getMagazine();
			if (val >= 0) {
				char buf[BUFFER_MAX];
				snprintf(buf, BUFFER_MAX, "%i", val);
				this->render->renderText(buf, 50, 50);
			} else if (val == -2) {
				this->render->renderText("relodn!", 50, 50);
			}

			val = this->ps->p->getBelt();
			if (val >= 0) {
				char buf[BUFFER_MAX];
				snprintf(buf, BUFFER_MAX, "%i", val);
				this->render->renderText(buf, 150, 50);
			}

			val = (int)floor(this->ps->p->getHealth());
			if (val >= 0) {
				char buf[BUFFER_MAX];
				snprintf(buf, BUFFER_MAX, "%i", val);
				this->render->renderText(buf, 50, 80);
			}
		}
	}

	// Crosshair
	if (this->ps->p != NULL && (this->render->viewmode == GameSettings::firstPerson || this->ps->p->getWeaponZoom() > 0.0f)) {
		WeaponType* wt = this->ps->p->getWeaponTypeCurr();
		if (wt->crosshair) {
			int size = wt->crosshair_min;		// TODO: Make dynamic based on accuracy

			int x = (int)round(((float)this->render->virt_width - (float)size) / 2.0f);
			int y = (int)round(((float)this->render->virt_height - (float)size) / 2.0f);

			glEnable(GL_BLEND);
			glUseProgram(render->shaders[SHADER_BASIC]->p());
			glUniformMatrix4fv(render->shaders[SHADER_BASIC]->uniform("uMVP"), 1, GL_FALSE, glm::value_ptr(render->ortho));
			this->render->renderSprite(wt->crosshair, x, y, size, size);
		}
	}
}


/**
* Up scroll or equivalent
**/
void HUD::eventUp()
{
	if (!this->ps->p->getNumWeapons()) return;
	this->weapon_menu = true;
	this->ps->p->setWeapon(this->ps->p->getPrevWeaponID());
}


/**
* Down scroll or equivalent
**/
void HUD::eventDown()
{
	if (!this->ps->p->getNumWeapons()) return;
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

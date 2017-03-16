// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include "hud.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "../game_engine.h"
#include "../game_state.h"
#include "../game_settings.h"
#include "../entity/unit/player.h"
#include "../mod/weapontype.h"
#include "../net/net_server.h"
#include "hud_label.h"
#include "render_opengl.h"


#define BUFFER_MAX 50
#define DISPLAY_TIME 1000*5;

using namespace std;



HUD::HUD(PlayerState *ps, RenderOpenGL *render)
{
	this->ps = ps;
	this->render = render;
	this->weapon_menu = false;
	this->weapon_menu_remove_time = 0;
}


void HUD::addMessage(string text1, string text2)
{
	HUDMessage *msg = new HUDMessage();
	msg->text = text1.append(text2);
	msg->remove_time = this->ps->st->game_time + DISPLAY_TIME;
	this->msgs.push_front(msg);
}


/**
* Add a data table to the HUD
**/
HUDLabel * HUD::addLabel(int x, int y, string data, HUDLabel *l)
{
	if (l == NULL) {
		l = new HUDLabel(x, y, data);
	}
	l->width = this->render->virt_width;
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
	glDisable(GL_DEPTH_TEST);

	OpenGLFont* font = this->render->getFont("DejaVuSans", 20.0f);
	if (font == NULL) {
		return;
	}

	if (this->weapon_menu && this->ps->p != NULL) {
		// Weapon menu
		if (this->weapon_menu_remove_time < ps->st->game_time) {
			this->weapon_menu = false;
		} else {
			int x = 100;
			int y = 100;
			unsigned int num = this->ps->p->getNumWeapons();

			for (unsigned int i = 0; i < num; i++) {
				WeaponType *wt = this->ps->p->getWeaponTypeAt(i);

				this->render->renderText(font, wt->title, x, y);

				if (i == this->ps->p->getCurrentWeaponID()) {
					this->render->renderText(font, ">", x - 25, y);
				}

				y += 30;
			}
		}
	} else {
		// Messages
		int y = this->render->virt_height;
		for (list<HUDMessage*>::iterator it = this->msgs.begin(); it != this->msgs.end(); ++it) {
			HUDMessage *msg = (*it);

			if (msg->remove_time < ps->st->game_time) {
				// TODO: Remove the message
				continue;
			}

			y -= 20;
			this->render->renderText(font, msg->text, 20, y);
		}

		// Labels
		int x;
		for (list<HUDLabel*>::iterator it = this->labels.begin(); it != this->labels.end(); ++it) {
			HUDLabel *l = (*it);
			// Send state over network
			if (! l->visible || l->a <= 0.001f) continue;
			if (GEng()->server != NULL) {
				GEng()->server->addmsgHUD(l);
			}

			x = l->x;
			y = l->y;
			if (x < 0) x = this->render->virt_width + x;
			if (y < 0) y = this->render->virt_height + y;
			
			if (l->align == ALIGN_LEFT) {
				this->render->renderText(font, l->data, x, y, l->r, l->g, l->b, l->a);

			} else if (l->align == ALIGN_CENTER) {
				int w = render->widthText(font, l->data);
				this->render->renderText(font, l->data, x + (l->width - w) / 2, y, l->r, l->g, l->b, l->a);

			} else if (l->align == ALIGN_RIGHT) {
				int w = render->widthText(font, l->data);
				this->render->renderText(font, l->data, x + (l->width - w), y, l->r, l->g, l->b, l->a);

			} else {
				cerr << "HUD label with unknown align: " << l->align << " : " << l->data << endl;
			}
		}

		// Workaround to remove "old" HUD labels when running as a client
		if (GEng()->client != NULL) {
			this->labels.clear();
		}

		// Powerup message
		if (this->ps->p != NULL) {
			string msg = this->ps->p->getPowerupMessage();
			if (!msg.empty()) {
				int w = render->widthText(font, msg);
				this->render->renderText(font, msg, (this->render->virt_width - w) / 2, this->render->virt_height - 50, 0.8f, 0.0f, 0.0f, 1.0f);
			}
		}

		// Health, ammo, etc
		if (this->ps->p != NULL) {
			int val;

			val = this->ps->p->getMagazine();
			if (val >= 0) {
				char buf[BUFFER_MAX];
				snprintf(buf, BUFFER_MAX, "%i", val);
				this->render->renderText(font, buf, 50, 50);
			} else if (val == -2) {
				this->render->renderText(font, "relodn!", 50, 50);
			}

			val = this->ps->p->getBelt();
			if (val >= 0) {
				char buf[BUFFER_MAX];
				snprintf(buf, BUFFER_MAX, "%i", val);
				this->render->renderText(font, buf, 150, 50);
			}

			val = (int)floor(this->ps->p->getHealth());
			if (val >= 0) {
				char buf[BUFFER_MAX];
				snprintf(buf, BUFFER_MAX, "%i", val);
				this->render->renderText(font, buf, 50, 80);
			}
		}
	}

	// Crosshair
	if (this->ps->p != NULL && this->render->effectiveViewmode() == GameSettings::firstPerson) {
		WeaponType* wt = this->ps->p->getWeaponTypeCurr();
		if (wt->crosshair) {
			int size = wt->crosshair_min;		// TODO: Make dynamic based on accuracy

			int x = (this->render->virt_width - size) / 2;
			int y = (this->render->virt_height - size) / 2;

			glEnable(GL_BLEND);
			glUseProgram(render->shaders[SHADER_BASIC]->p());
			glUniformMatrix4fv(render->shaders[SHADER_BASIC]->uniform("uMVP"), 1, GL_FALSE, glm::value_ptr(render->ortho));
			this->render->renderSprite(wt->crosshair, x, y, size, size);
		}
	}

	glEnable(GL_DEPTH_TEST);
}


/**
* Up scroll or equivalent
**/
void HUD::eventUp()
{
	if (!this->ps->p->getNumWeapons()) return;
	this->weapon_menu = true;
	this->ps->p->setWeapon(this->ps->p->getPrevWeaponID());
	this->weapon_menu_remove_time = this->ps->st->game_time + DISPLAY_TIME;
}


/**
* Down scroll or equivalent
**/
void HUD::eventDown()
{
	if (!this->ps->p->getNumWeapons()) return;
	this->weapon_menu = true;
	this->ps->p->setWeapon(this->ps->p->getNextWeaponID());
	this->weapon_menu_remove_time = this->ps->st->game_time + DISPLAY_TIME;
}


/**
* Click or equivalent
**/
void HUD::eventClick()
{
	this->weapon_menu = false;
	this->weapon_menu_remove_time = 0;
}

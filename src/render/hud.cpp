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


void HUD::addAlertMessage(string text)
{
	AlertMessage *msg = new AlertMessage();
	msg->text = text;
	msg->remove_time = this->ps->st->game_time + 5000;
	this->msgs.push_front(msg);
}

void HUD::addAlertMessage(string text1, string text2)
{
	AlertMessage *msg = new AlertMessage();
	msg->text = text1.append(text2);
	msg->remove_time = this->ps->st->game_time + 5000;
	this->msgs.push_front(msg);
}


/**
* Add a data table to the HUD
**/
int HUD::addDataTable(int x, int y, int cols, int rows)
{
	DataTable * dt;
	
	dt = new DataTable();
	dt->x = x;
	dt->y = y;
	dt->cols = cols;
	dt->rows = rows;
	dt->visible = true;
	
	for (int i = 0; i < (cols * rows); i++) {
		dt->data.push_back("");
	}
	
	this->tables.push_back(dt);
	
	return this->tables.size() - 1;
}

/**
* Set a value of a data table
**/
void HUD::setDataValue(int table_id, int col, int row, string val)
{
	DataTable * dt;
	dt = this->tables.at(table_id);
	dt->data[(row * dt->cols) + col] = val;
}

/**
* Remove a data table
**/
void HUD::removeDataTable(int table_id)
{
	DataTable * dt;
	dt = this->tables.at(table_id);
	dt->visible = false;
}

/**
* Remove a data table
**/
void HUD::removeAllDataTables()
{
	this->tables.clear();
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
		// Alert messages
		int y = 1000;
		for (list<AlertMessage*>::iterator it = this->msgs.begin(); it != this->msgs.end(); it++) {
			AlertMessage *msg = (*it);
			
			if (msg->remove_time < ps->st->game_time) {
				// TODO: Remove the message
				continue;
			}
			
			y -= 20;
			render->renderText(msg->text, 20, y);
		}
		
		// Data tables
		for (unsigned int i = 0; i < this->tables.size(); i++) {
			DataTable *dt = this->tables[i];
			if (! dt->visible) continue;
			
			for (int col = 0; col < dt->cols; col++) {
				for (int row = 0; row < dt->rows; row++) {
					render->renderText(dt->data[(row * dt->cols) + col], dt->x + (100 * col), dt->y + (20 * row));
				}
			}
		}
		
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

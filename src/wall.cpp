// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include <math.h>
#include "rage.h"

using namespace std;


Wall::Wall(WallType *wt, GameState *st) : Entity(st)
{
	this->wt = wt;
	this->anim = new AnimPlay(wt->model);
	this->health = 10000;
}

Wall::~Wall()
{
	delete (this->anim);
}


/**
* Do stuff
**/
void Wall::update(int delta)
{
	if (this->anim->isDone()) this->anim->next();
	
	this->st->addCollideBox((int) this->x, (int) this->y, 30);
}

/**
* NOT USED
**/
void Wall::getSprite(SpritePtr list [SPRITE_LIST_LEN])
{
	list[0] = NULL;
}

/**
* Draw something
**/
void Wall::getAnimModel(AnimPlay * list [SPRITE_LIST_LEN])
{
	list[0] = this->anim;
	list[1] = NULL;
}

/**
* Handle events
**/
void Wall::handleEvent(Event * ev)
{
}



/**
* We have been hit! Take some damage
**/
void Wall::takeDamage(int damage)
{
	this->health -= damage;
	if (this->health < 0) this->health = 0;
	
	for (unsigned int j = 0; j < this->wt->damage_models.size(); j++) {
		WallTypeDamage * dam = this->wt->damage_models.at(j);
		
		if (this->health <= dam->health) {
			delete(this->anim);
			this->anim = new AnimPlay(dam->model);
			break;
		}
	}
}


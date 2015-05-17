// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include "touch.h"
#include "events.h"
#include "game_state.h"
#include "entity/player.h"
#include "util/util.h"

static int numberOfFingers = 0;


static bool insideShootArea(float x, float y)
{
	if (x < 0.6f && x > 0.4f && y < 0.6f && y > 0.4f) { // min=0, max=1
		return true;
	}
	return false;
}


/**
* A finger has touched onto the device
**/
void fingerDown(GameState *st, float x, float y)
{
	numberOfFingers++;

	if (insideShootArea(x, y)) {
		st->local_players[0]->p->keyPress(Player::KEY_FIRE);
	} else {
		st->local_players[0]->p->keyRelease(Player::KEY_FIRE);
	}
}


/**
* A finger has moved
**/
void fingerMove(GameState *st, int finger, float x, float y)
{
	//displayMessageBox("fingerMove " + numberToString(finger) + ": " + numberToString(x) + "x" + numberToString(y));

	if (finger > numberOfFingers) {
		numberOfFingers = finger;
	}

	if (numberOfFingers <= 2) {
		const float middlePoint = 0.5f;

		if (x < middlePoint) {

			// Left side of screen: Move
			const float moveOffset = 0.1f;
			const float middleLeft = middlePoint / 2.0f;

			if (x + moveOffset < middleLeft) {
				st->local_players[0]->p->keyPress(Player::KEY_LEFT);
				st->local_players[0]->p->keyRelease(Player::KEY_RIGHT);
			} else if (x - moveOffset > middleLeft) {
				st->local_players[0]->p->keyPress(Player::KEY_RIGHT);
				st->local_players[0]->p->keyRelease(Player::KEY_LEFT);
			} else {
				st->local_players[0]->p->keyRelease(Player::KEY_RIGHT);
				st->local_players[0]->p->keyRelease(Player::KEY_LEFT);
			}
			if (y + moveOffset < middlePoint) {
				st->local_players[0]->p->keyPress(Player::KEY_UP);
				st->local_players[0]->p->keyRelease(Player::KEY_DOWN);
			} else if (y - moveOffset > middlePoint) {
				st->local_players[0]->p->keyPress(Player::KEY_DOWN);
				st->local_players[0]->p->keyRelease(Player::KEY_UP);
			} else {
				st->local_players[0]->p->keyRelease(Player::KEY_DOWN);
				st->local_players[0]->p->keyRelease(Player::KEY_UP);
			}

		} else {

			// Right side of screen: Aim
			const float middleRight = middlePoint + middlePoint / 2.0f; // min=0, max=1
			const float factor = 25.0f;

			game_x[0] += (x - middleRight) * factor;
			net_x[0]  += (x - middleRight) * factor;

			game_y[0] += (y - middlePoint) * factor;
			net_y[0]  += (y - middlePoint) * factor;
		}
	}
}


/**
* Multiple finger gesture event
**/
void multigesture(GameState *st, unsigned int fingers, float x, float y)
{
	//displayMessageBox("multigusture " + numberToString(fingers) + ": " + numberToString(x) + "x" + numberToString(y));

	numberOfFingers = static_cast<int>(fingers);

	if (numberOfFingers <= 2) {
		fingerMove(st, numberOfFingers, x, y);
	} else if (numberOfFingers == 3) {
		st->local_players[0]->p->keyPress(Player::KEY_USE);
	} else if (numberOfFingers == 5) {
		st->gs->switchViewMode();
	} else {
		st->local_players[0]->p->reload();
	}
}


/**
* A finger has raised off the device
**/
void fingerUp(GameState *st)
{
	numberOfFingers--;
	if (numberOfFingers < 0) {
		numberOfFingers = 0;
	}

	if (numberOfFingers == 0) {
		st->local_players[0]->p->resetKeyPress();
	}
}

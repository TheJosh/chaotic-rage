// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include "touch.h"
#include <SDL.h>
#include <string>

#include "events.h"
#include "game_state.h"
#include "entity/player.h"
#include "util/util.h"

using namespace std;

static int numberOfFingers = 0;

/**
* A finger has touched onto the device
**/
void fingerDown(GameState *st, int finger, float x, float y)
{
	//displayMessageBox("fingerDown " + numberToString(finger));

	numberOfFingers++;
}

/**
* A finger has moved
**/
void fingerMove(GameState *st, int finger, float x, float y, float dx, float dy)
{
	//displayMessageBox("fingerMove " + numberToString(finger) + ": " + numberToString(x) + "x" + numberToString(y) + " delta: " + numberToString(dx) + "x" + numberToString(dy));

	if (finger > numberOfFingers) {
		numberOfFingers = finger;
	}

	if (numberOfFingers == 1) {
		if (x < 0.6f && x > 0.4f && y < 0.6f && y > 0.4f) { // min=0, max=1
			// Shoot if the middle of the screen is touched using only one finger.
			st->local_players[0]->p->keyPress(Player::KEY_FIRE);
		} else {
			st->local_players[0]->p->keyRelease(Player::KEY_FIRE);

			// move
			const float middlePoint = 0.5f;
			if (x < middlePoint) {
				st->local_players[0]->p->keyPress(Player::KEY_LEFT);
				st->local_players[0]->p->keyRelease(Player::KEY_RIGHT);
			} else {
				st->local_players[0]->p->keyPress(Player::KEY_RIGHT);
				st->local_players[0]->p->keyRelease(Player::KEY_LEFT);
			}
			if (y < middlePoint) {
				st->local_players[0]->p->keyPress(Player::KEY_UP);
				st->local_players[0]->p->keyRelease(Player::KEY_DOWN);
			} else {
				st->local_players[0]->p->keyPress(Player::KEY_DOWN);
				st->local_players[0]->p->keyRelease(Player::KEY_UP);
			}
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

	if (numberOfFingers == 2) {
		// aim
		const float offset = 0.5f; // min=0, max=1
		const float factor = 5.0f;

		game_x[0] += (offset - x) * factor;
		net_x[0]  += (offset - x) * factor;

		game_y[0] += (offset - y) * factor;
		net_y[0]  += (offset - y) * factor;
	}
}


/**
* A finger has raised off the device
**/
void fingerUp(GameState *st, int finger, float x, float y)
{
	//displayMessageBox("fingerUp" + numberToString(finger));

	numberOfFingers--;
	if (numberOfFingers < 0) {
		numberOfFingers = 0;
	}

	st->local_players[0]->p->keyRelease(Player::KEY_FIRE);

	st->local_players[0]->p->keyRelease(Player::KEY_LEFT);
	st->local_players[0]->p->keyRelease(Player::KEY_RIGHT);
	st->local_players[0]->p->keyRelease(Player::KEY_UP);
	st->local_players[0]->p->keyRelease(Player::KEY_DOWN);
}

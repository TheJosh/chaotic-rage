// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <SDL.h>
#include <string>

#include "events.h"
#include "game_state.h"
#include "entity/player.h"
#include "touch.h"

using namespace std;


/**
* A finger has touched onto the device
**/
void fingerDown(GameState *st, int finger, float x, float y)
{
	fingerMove(st, finger, x, y);
}


/**
* A finger has moved
**/
void fingerMove(GameState *st, int finger, float x, float y)
{
	int type = 0;

	// Determine type (1 = left thumb, 2 = right thumb)
	if (y > 0.9f) {
		y = (y - 0.9f) * 10.0f;

		if (x < 0.1f) {
			x = x * 10.0f;
			type = 1;
		} else if (x > 0.9f) {
			x = (x - 0.9f) * 10.0f;
			type = 2;
		}
	}

	if (type == 1) {
		// Left thumb (type 1) = move
		if (x < 0.3) {
			st->local_players[0]->p->keyPress(Player::KEY_LEFT);
		} else if (x > 0.7) {
			st->local_players[0]->p->keyPress(Player::KEY_RIGHT);
		}

		if (y < 0.3) {
			st->local_players[0]->p->keyPress(Player::KEY_UP);
		} else if (y > 0.7) {
			st->local_players[0]->p->keyPress(Player::KEY_DOWN);
		}

	} else if (type == 2) {
		// Left thumb (type 1) = aim
		game_x[0] += x * 20.0f;
		net_x[0] += x * 20.0f;
		game_y[0] += y * 20.0f;
		net_y[0] += y * 20.0f;
	}
}


/**
* A finger has raised off the device
**/
void fingerUp(GameState *st, int finger, float x, float y)
{
}


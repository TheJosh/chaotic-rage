#pragma once

class GameState;
class Player;
class HUD;


/**
* Info about the players in the game.
**/
class PlayerState
{
	public:
		GameState* st;
		Player* p;				// Player entity
		HUD* hud;				// Heads-up display rendering (data tables, messages, etc)
		unsigned int slot;		// Slots are numbered from 1 upwards. Used for network play.

	public:
		explicit PlayerState(GameState *st);
		~PlayerState();

	private:
		PlayerState(const PlayerState& disabledCopyConstuctor);
};

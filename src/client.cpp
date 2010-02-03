#include <iostream>
#include <SDL.h>
#include "entity.h"
#include "unit.h"
#include "player.h"
#include "gamestate.h"

using namespace std;



int main (int argc, char ** argv) {
	
	Player *u = new Player();
	
	u->x = 100;
	u->getSprite();
	u->update(5);
	
	
	GameState *st = new GameState();
	st->addUnit(u);
	
	int i;
	for (i = 0; i < st->numUnits(); i++) {
		st->getUnit(i)->update(5);
	}
	
	printf("%i\n", st->getUnit(0)->x);
	
	st->getUnit(0)->update(5);
	printf("%i\n", st->getUnit(0)->x);
}






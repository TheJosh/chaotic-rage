#include "player_state.h"
#include "game_engine.h"
#include "render_opengl/hud.h"

class GameState;
class RenderOpenGL;


PlayerState::PlayerState(GameState *st)
{
	this->st = st;
	this->p = NULL;
	this->hud = new HUD(this, reinterpret_cast<RenderOpenGL*>(GEng()->render));
	this->slot = 0;
}

PlayerState::~PlayerState()
{
	delete(this->hud);
}

#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"


class Render
{
	public:
		virtual void setScreenSize(int width, int height, bool fullscreen) = 0;
		virtual void render(GameState *st) = 0;
};

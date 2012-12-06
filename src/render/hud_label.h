// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include "../rage.h"

using namespace std;


enum HUDLabelAlign {
	ALIGN_LEFT = 1,
	ALIGN_CENTER = 2,
	ALIGN_RIGHT = 3
};


class HUDLabel
{
	public:
		int x;
		int y;
		int width;
		HUDLabelAlign align;
		string data;
		bool visible;

	public:
		HUDLabel(int x, int y, string data)
			: x(x), y(y), data(data), width(0), visible(true), align(ALIGN_LEFT)
			{};
};

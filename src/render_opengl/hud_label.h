// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once

using namespace std;


enum HUDLabelAlign {
	ALIGN_LEFT = 1,
	ALIGN_CENTER = 2,
	ALIGN_RIGHT = 3
};


class HUDLabel
{
	public:
		float x;
		float y;
		float width;
		HUDLabelAlign align;
		string data;
		bool visible;
		float r;
		float g;
		float b;
		float a;

	public:
		HUDLabel(float x, float y, string data, HUDLabelAlign align = ALIGN_LEFT,
			float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f)
			: x(x), y(y), width(0.0f), align(align), data(data), visible(true),
			  r(r), g(g), b(b), a(a)
			{};
};

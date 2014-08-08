// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once


class Light {
	public:
		unsigned int type;		// 1, 2 or 3
		float x;
		float y;
		float z;
		float diffuse [4];
		float specular [4];

	public:
		Light(unsigned int type);
		~Light();

		void setDiffuse(short r, short g, short b, short a);
		void setSpecular(short r, short g, short b, short a);
};

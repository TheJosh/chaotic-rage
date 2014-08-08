// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;


#include "light.h"



Light::Light(unsigned int type)
{
	this->type = type;
	this->x = this->y = this->z = 0.0f;
	this->diffuse[0] = this->diffuse[1] = this->diffuse[2] = this->diffuse[3] = 0.0;
	this->specular[0] = this->specular[1] = this->specular[2] = this->specular[3] = 0.0;
}


Light::~Light()
{
}


/**
* Set diffuse color
* Red, Green, Blue, Alpha in range 0 to 255
**/
void Light::setDiffuse(short r, short g, short b, short a)
{
	this->diffuse[0] = r / 255.0f;
	this->diffuse[1] = g / 255.0f;
	this->diffuse[2] = b / 255.0f;
	this->diffuse[3] = a / 255.0f;
}


/**
* Set specular color
* Red, Green, Blue, Alpha in range 0 to 255
**/
void Light::setSpecular(short r, short g, short b, short a)
{
	this->specular[0] = r / 255.0f;
	this->specular[1] = g / 255.0f;
	this->specular[2] = b / 255.0f;
	this->specular[3] = a / 255.0f;
}

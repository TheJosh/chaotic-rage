// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include "../rage.h"
#include "strings.h"
#include <string>


/***
* This is a simple implementation of an i18n system
***/


/**
* Macro to make it quick and easy to use strings
**/
#define _ gettext


/**
* Return the translated version of a given string
**/
std::string gettext(unsigned int stringid);


/**
* Load a language file
**/
bool loadLang(const char* name);


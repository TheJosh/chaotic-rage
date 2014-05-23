// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include "../rage.h"

#include <string>
#include <map>
#include "string.h"
#define BUFFER_MAX 255


/***
* This is a simple implementation of an i18n system
***/


/**
* Stores the loaded strings
**/
static std::map<int, std::string> strings;


/**
* Load a language file
**/
bool loadLang(const char* name)
{
	char buf[BUFFER_MAX];
	
	strcpy(buf, "data/i18n/");
	strcat(buf, name);
	strcat(buf, ".txt");
	
	FILE* fp = fopen(buf, "rb");
	if (fp == NULL) return false;
	
	while (fgets(buf, BUFFER_MAX, fp)) {
		char* tab = strchr(buf, '\t');
		char* nl = strchr(buf, '\n');
		*nl = '\0';
		strings.insert(std::pair<int,std::string>(atoi(buf), std::string(tab+1)));
	}
	
	fclose(fp);
	
	return true;
}


/**
* Return the translated version of a given string
**/
std::string gettext(unsigned int stringid)
{
	return strings[stringid];
}


/**
* Get a list of available languages
* It's hardcoded but I don't think that matters
**/
std::vector<std::string>* getAvailableLangs()
{
	std::vector<std::string> *out = new std::vector<std::string>();
	out->push_back("en");
	out->push_back("en_Pirate");
	return out;
}
// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include "gettext.h"
#include <iostream>
#include <map>
#include "../mod/mod.h"


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
	char buf2[BUFFER_MAX];

	Mod* mod = new Mod(NULL, "data/i18n");

	// Read strings
	std::string filename = std::string(name);
	filename.append(".txt");
	char* input = mod->loadText(filename);
	if (input == NULL) {
		delete mod;
		std::cerr << "Error: Could not load language file: " << buf << std::endl;
		return false;
	}

	// Parse strings
	strings.clear();
	char* ptr = input;
	char* curr = buf;
	int len = 0;
	while (*ptr != 0) {
		if (*ptr == '\t') {
			memset(buf2, 0, BUFFER_MAX);
			curr = buf2;
			len = 0;
		} else if (*ptr == '\r' || *ptr == '\n') {
			strings.insert(std::pair<int,std::string>(atoi(buf), std::string(buf2)));
			memset(buf, 0, BUFFER_MAX);
			curr = buf;
			len = 0;
		} else {
			++len;
			if (len == BUFFER_MAX) {
				free(input);
				delete mod;
				std::cerr << "Error: Could not load language file; string '" << curr << "' too long" << std::endl;
				return false;
			}
			*curr = *ptr;
			++curr;
		}
		++ptr;
	}

	free(input);
	delete mod;

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
* TODO: Read file names from i18n directory
**/
std::vector<Lang>* getAvailableLangs()
{
	std::vector<Lang> *out = new std::vector<Lang>();
	out->push_back(Lang("en", "English"));
	out->push_back(Lang("es", "Español"));
	out->push_back(Lang("eo", "Esperanto"));
	out->push_back(Lang("fr", "Français"));
	out->push_back(Lang("en_Pirate", "Pirate"));
	out->push_back(Lang("pt_br", "Português Brasileiro"));
	out->push_back(Lang("sv", "Svenska"));
#ifndef RELEASE
	out->push_back(Lang("test_utf8", "UTF-8 Test"));
#endif
	return out;
}


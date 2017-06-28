// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <glm/glm.hpp>
#include <string>
#include "../rage.h"

using namespace std;


class Mod;


/**
* A lightweight handle to the metadata for a map without
* requiring the loading of the whole map
**/
class MapReg {
	friend class MapRegistry;

	protected:
		string name;
		string title;
		Mod* mod;
		bool arcade;
		string preview;     // full path to preview image

	public:
		MapReg() : name(""), title(""), mod(NULL), arcade(true), preview("") {}
		MapReg(string name, string title) : name(name), title(title), mod(NULL), arcade(true), preview("") {}
		MapReg(string name, string title, Mod* mod) : name(name), title(title), mod(mod), arcade(true), preview("") {}
		MapReg(string name, string title, Mod* mod, bool arcade) : name(name), title(title), mod(mod), arcade(arcade), preview("") {}

	public:
		string getName() { return this->name; }
		string getTitle() { return this->title; }
		Mod* getMod() { return this->mod; }
		bool getArcade() { return this->arcade; }
		void setPreview(string preview) { this->preview = preview; }
		string getPreview() { return this->preview; }
};


/**
* The map registry
**/
class MapRegistry {
	public:
		vector<MapReg> maps;

	public:
		void find(string dir);
		void find(Mod* mod);
		MapReg* get(string name);

		string nameAt(int index) { return maps.at(index).name; }
		string titleAt(int index) { return maps.at(index).title; }
		MapReg* at(int index) { return &maps.at(index); }
		unsigned int size() { return maps.size(); }
};

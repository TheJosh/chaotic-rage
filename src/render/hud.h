// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include "../rage.h"

using namespace std;


class HUDMessage
{
	friend class HUD;
	private:
		string text;
		unsigned int remove_time;
};

class DataTable
{
	friend class HUD;
	private:
		int x;
		int y;
		int cols;
		int rows;
		vector<string> data;
		bool visible;
};


class HUD {
	public:
		PlayerState *ps;
		
	private:
		bool weapon_menu;
		list<HUDMessage*> msgs;
		vector<DataTable*> tables;
		
	public:
		HUD(PlayerState *ps);
	
	public:
		void render(Render3D * render);

		void eventClick();
		void eventUp();
		void eventDown();

		void addMessage(string text);
		void addMessage(string text1, string text2);
		
		int addDataTable(int x, int y, int cols, int rows);
		void setDataValue(int table_id, int col, int row, string val);
		void removeDataTable(int table_id);
		void removeAllDataTables();
};



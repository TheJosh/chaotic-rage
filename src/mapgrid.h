// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;



/**
* The cell size, in pixels
**/
#define MAPGRID_CELL_SIZE 60.0


/**
* A grid of cells for storing data in a way that makes
* getting data local to a specific area fairly fast
* and efficent.
*
* TODO: Maybe rehash this to a template which accepts a custom class for the cell
**/
class MapGrid
{
	private:
		vector<MapGridCell*> cells;
		int num_cells;
		int row_width;
		int row_height;
		
	public:
		MapGrid(int map_width, int map_height);
		~MapGrid();
		
	public:
		/**
		* Gets all cells
		**/
		vector<MapGridCell*>* getAllCells();
		
		//
		// Functions which take map co-ordinates for arguments
		//
		
		/**
		* Gets the cell at an x/y co-ordinate pair
		* Good for adding
		**/
		MapGridCell* getCellMC(int x, int y);
		
		/**
		* Gets all cells within a search radius
		* Good for searching
		* Don't forget to 'delete' the result when you are done, if "release" has been set to 1
		**/
		list<CollideBox*>* getCollidesMC(float x, float y, float radius, int *release);
};


/**
* A single cell of data on the grid.
**/
class MapGridCell
{
	public:
		list<CollideBox*> collideboxes;
		~MapGridCell();
};


/**
* A collide box.
**/
class CollideBox {
	public:
		int x;
		int y;
		int radius;
		Entity *e;
		bool del;
		
	public:
		CollideBox(int x, int y, int radius, Entity * e) {
			this->x = x;
			this->y = y;
			this->radius = radius;
			this->e = e;
			this->del = false;
		};
};



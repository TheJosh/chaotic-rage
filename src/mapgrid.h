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
#define MAPGRID_CELL_SIZE 20.0


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
		**/
		vector<MapGridCell*>* getCellsMC(int x, int y, int radius);
};


/**
* A single cell of data on the grid.
**/
class MapGridCell
{
	friend class MapGrid;
	
	protected:
		list<int> collideboxes;
};



// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include <math.h>
#include "rage.h"


using namespace std;


MapGrid::MapGrid(int map_width, int map_height)
{
	row_width = (int) ceil(map_width / MAPGRID_CELL_SIZE);
	row_height = (int) ceil(map_height / MAPGRID_CELL_SIZE);
	num_cells = row_width * row_height;
	
	for (int i = 0; i <= num_cells; i++) {
		this->cells.push_back(new MapGridCell());
	}
}

MapGrid::~MapGrid()
{
	for (int i = 0; i < num_cells; i++) {
		delete (this->cells.at(i));
	}
}

MapGridCell::~MapGridCell() {
}


/**
* Good for adding
**/
MapGridCell* MapGrid::getCellMC(int x, int y)
{
	x = (int) floor(x / MAPGRID_CELL_SIZE);
	y = (int) floor(y / MAPGRID_CELL_SIZE);
	
	return this->cells[y * row_width + x];
}


/**
* Good for searching
**/
list<CollideBox*>* MapGrid::getCollidesMC(float x, float y, float radius, int *release)
{
	int x1 = (int) floor((x - radius) / MAPGRID_CELL_SIZE);
	int y1 = (int) floor((y - radius) / MAPGRID_CELL_SIZE);
	int x2 = (int) floor((x + radius) / MAPGRID_CELL_SIZE);
	int y2 = (int) floor((y + radius) / MAPGRID_CELL_SIZE);
	
	if (x1 < 0) x1 = 0;
	if (y1 < 0) y1 = 0;
	if (x2 > row_width) x2 = row_width;
	if (y2 > row_height) y2 = row_height;
	
	if (x1 == x2 and y1 == y2) {
		*release = 0;
		return &(this->cells[y1 * row_width + x1]->collideboxes);
	}
	
	list<CollideBox*> * ret = new list<CollideBox*>();
	for (x = x1; x <= x2; x++) {
		for (y = y1; y <= y2; y++) {
			ret->insert (
				ret->end(),
				this->cells[y * row_width + x]->collideboxes.begin(),
				this->cells[y * row_width + x]->collideboxes.end()
			);
		}
	}
	
	*release = 1;
	return ret;
}


/**
* Good for base iteration
**/
vector<MapGridCell*>* MapGrid::getAllCells()
{
	return &(this->cells);
}



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
	num_cells = row_width * (int) ceil(map_height / MAPGRID_CELL_SIZE);
	
	for (int i = 0; i < num_cells; i++) {
		this->cells.push_back(new MapGridCell());
	}
}

MapGrid::~MapGrid()
{
	for (int i = 0; i < num_cells; i++) {
		delete (this->cells.at(i));
	}
}


/**
* Good for adding
**/
MapGridCell* MapGrid::getCellMC(int x, int y)
{
	x = (int) ceil(x / MAPGRID_CELL_SIZE);
	y = (int) ceil(y / MAPGRID_CELL_SIZE);
	
	return this->cells[y * row_width + x];
}


/**
* Good for searching
**/
vector<MapGridCell*>* MapGrid::getCellsMC(int x, int y, int radius)
{
	vector<MapGridCell*> * ret = new vector<MapGridCell*>();
	
	x = (int) ceil(x / MAPGRID_CELL_SIZE);
	y = (int) ceil(y / MAPGRID_CELL_SIZE);
	radius = (int) ceil(radius / MAPGRID_CELL_SIZE);
	
	int x2 = x + radius;
	int y2 = y + radius;
	
	for (x -= radius; x < x2; x++) {
		for (y -= radius; y < y2; y++) {
			ret->push_back(this->cells[y * row_width + x]);
		}
	}
	
	return ret;
}


/**
* Good for base iteration
**/
vector<MapGridCell*>* MapGrid::getAllCells()
{
	return &(this->cells);
}



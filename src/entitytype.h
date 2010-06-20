#pragma once
#include <iostream>
#include <SDL.h>
#include "rage.h"

using namespace std;


class EntityType
{
	public:
		vector<Action*>* actions;
		
	protected:
		EntityType();
		~EntityType();
		
	public:
		void doActions(Entity * entity, ActionEvent event);
};




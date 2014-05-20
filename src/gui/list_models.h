// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <vector>
#include <guichan.hpp>
#include "../map/map.h"
#include "../mod/mod.h"
#include "../mod/gametype.h"


using namespace std;


class MapRegistry;
class GameType;
class Mod;
class GameManager;
class GameType;


class VectorListModel: public gcn::ListModel
{
	private:
		vector<string> * v;

	public:
		explicit VectorListModel(vector<string> * vec) {
			this->v = vec;
		}

		std::string getElementAt(int i)
		{
			return v->at(i);
		}

		int getNumberOfElements()
		{
			return v->size();
		}
};


class MapRegistryListModel: public gcn::ListModel
{
	private:
		MapRegistry * mapreg;

	public:
		explicit MapRegistryListModel(MapRegistry * mapreg) : mapreg(mapreg) {}

		std::string getElementAt(int i)
		{
			return mapreg->titleAt(i);
		}

		int getNumberOfElements()
		{
			return mapreg->size();
		}
};


class GametypeListModel: public gcn::ListModel
{
	private:
		vector<GameType*> * gametypes;

	public:
		explicit GametypeListModel(vector<GameType*> * gametypes) : gametypes(gametypes) {}
		~GametypeListModel() { delete(gametypes); }

		std::string getElementAt(int i)
		{
			return gametypes->at(i)->title;
		}

		int getNumberOfElements()
		{
			return gametypes->size();
		}
};


class GametypeFactionsListModel: public gcn::ListModel
{
	private:
		GameType* gt;

	public:
		explicit GametypeFactionsListModel(GameType* gt) : gt(gt) {}
		~GametypeFactionsListModel() {}

		std::string getElementAt(int i)
		{
			return gt->factions[i].title;
		}

		int getNumberOfElements()
		{
			return gt->num_factions;
		}
};


class ModListModel: public gcn::ListModel
{
	private:
		vector<Mod*> * mods;

	public:
		explicit ModListModel(vector<Mod*> * mods) : mods(mods) {}
		~ModListModel() { delete(mods); }

		std::string getElementAt(int i)
		{
			return mods->at(i)->getTitle();
		}

		Mod* getModAt(int i)
		{
			return mods->at(i);
		}

		int getNumberOfElements()
		{
			return mods->size();
		}

		unsigned int findMod(Mod* mod)
		{
			for (unsigned int i = 0; i < mods->size(); i++) {
				if (mods->at(i)->name == mod->name) return i;
			}
			return 0;
		}
};


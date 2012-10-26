// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <string>
#include <SDL.h>
#include <confuse.h>
#include <zzip/zzip.h>
#include <map>
#include "../rage.h"

using namespace std;


// This is to save loading the same mesh into memory multiple times
// especially important for animation, etc.
static map <string, WavefrontObj *> loaded_meshes;
static map <string, SpritePtr> loaded_textures;



/**
* Config file opts
**/
// MeshFrame
cfg_opt_t _meshframe_opts[] =
{
	CFG_INT((char*) "frame", 0, CFGF_NONE),
	CFG_STR((char*) "mesh", 0, CFGF_NONE),
	CFG_STR((char*) "texture", 0, CFGF_NONE),

	CFG_FLOAT((char*) "px", 0, CFGF_NONE),
	CFG_FLOAT((char*) "py", 0, CFGF_NONE),
	CFG_FLOAT((char*) "pz", 0, CFGF_NONE),
	CFG_FLOAT((char*) "rx", 0, CFGF_NONE),
	CFG_FLOAT((char*) "ry", 0, CFGF_NONE),
	CFG_FLOAT((char*) "rz", 0, CFGF_NONE),
	CFG_FLOAT((char*) "s", 1, CFGF_NONE),
	CFG_FLOAT((char*) "sx", 1, CFGF_NONE),
	CFG_FLOAT((char*) "sy", 1, CFGF_NONE),
	CFG_FLOAT((char*) "sz", 1, CFGF_NONE),
	
	CFG_INT((char*) "do-angle", 1, CFGF_NONE),
	
	CFG_INT_LIST((char*) "emission", 0, CFGF_NONE),
	CFG_END()
};

// AnimModel
cfg_opt_t animmodel_opts[] =
{
	CFG_STR((char*) "name", 0, CFGF_NONE),
	CFG_INT((char*) "num_frames", 0, CFGF_NONE),
	CFG_SEC((char*) "meshframe", _meshframe_opts, CFGF_MULTI),
	CFG_STR((char*) "next", (char*)"", CFGF_NONE),
	CFG_STR((char*) "mesh", 0, CFGF_NONE),
	CFG_STR((char*) "texture", 0, CFGF_NONE),
	CFG_END()
};


/**
* Loads a mesh from storage. Does some clever caching to only load it once
**/
WavefrontObj * cachedLoadMesh(string name, Mod * mod)
{
	WavefrontObj * ret = NULL;

	map<string, WavefrontObj *>::iterator it = loaded_meshes.find(name);
	if (it == loaded_meshes.end()) {
		string filename = mod->directory;
		filename.append("animmodels/");
		filename.append(name);
		filename.append(".obj");
		
		ret = loadObj(filename);
		
		if (ret == NULL) {
			cerr << "Bad mesh: " << filename << "\n";
			return NULL;
		}
		
		ret->calcBoundingSize();
		
		loaded_meshes[name] = ret;
		
	} else {
		ret = it->second;
	}

	return ret;
}


/**
* Loads a texture from storage. Does some clever caching to only load it once
**/
SpritePtr cachedLoadTexture(string name, Mod * mod)
{
	SpritePtr tex = NULL;

	map<string, SpritePtr>::iterator it = loaded_textures.find(name);
	if (it == loaded_textures.end()) {
		string filename = "animmodels/";
		filename.append(name);
		filename.append(".png");
				
		tex = mod->st->render->loadSprite(filename, mod);
				
		if (tex == NULL) { cerr << "Bad texture: " << filename << "\n"; return NULL; }
				
		loaded_textures[name] = tex;
	} else {
		tex = it->second;
	}

	return tex;
}


/**
* Item loading function handler
**/
AnimModel* loadItemAnimModel(cfg_t *cfg_model, Mod * mod)
{
	AnimModel* am;
	string filename;
	cfg_t *cfg_meshframe;
	
	am = new AnimModel();
	am->name = cfg_getstr(cfg_model, "name");
	am->num_frames = cfg_getint(cfg_model, "num_frames");
	am->next_name = cfg_getstr(cfg_model, "next");
	
	// If a model doesn't provide num_frames arg, we assume a single simple frame
	// with just a mesh and a texture. Nice an quick to work with.
	if (am->num_frames == 0) {
		am->num_frames = 1;

		MeshFrame* mf = new MeshFrame();
		mf->frame = 0;
		mf->mesh = cachedLoadMesh(cfg_getstr(cfg_model, "mesh"), mod);
		mf->texture = cachedLoadTexture(cfg_getstr(cfg_model, "texture"), mod);
		mf->texture_name = cfg_getstr(cfg_model, "texture");

		mf->px = mf->py = mf->pz = 0;
		mf->rx = mf->ry = mf->rz = 0;
		mf->sx = mf->sy = mf->sz = 1;
		mf->do_angle = 0;

		am->meshframes.push_back(mf);

		return am;
	}


	int num = cfg_size(cfg_model, "meshframe");
	int j,k;
	for (j = 0; j < num; j++) {
		cfg_meshframe = cfg_getnsec(cfg_model, "meshframe", j);
		
		MeshFrame* mf = new MeshFrame();
		mf->frame = cfg_getint(cfg_meshframe, "frame");
		
		mf->mesh = cachedLoadMesh(cfg_getstr(cfg_meshframe, "mesh"), mod);
		mf->texture = cachedLoadTexture(cfg_getstr(cfg_meshframe, "texture"), mod);
		mf->texture_name = cfg_getstr(cfg_meshframe, "texture");
		
		mf->px = cfg_getfloat(cfg_meshframe, "px");
		mf->py = cfg_getfloat(cfg_meshframe, "py");
		mf->pz = cfg_getfloat(cfg_meshframe, "pz");
		mf->rx = cfg_getfloat(cfg_meshframe, "rx");
		mf->ry = cfg_getfloat(cfg_meshframe, "ry");
		mf->rz = cfg_getfloat(cfg_meshframe, "rz");
		mf->sx = cfg_getfloat(cfg_meshframe, "sx");
		mf->sy = cfg_getfloat(cfg_meshframe, "sy");
		mf->sz = cfg_getfloat(cfg_meshframe, "sz");
		
		{
			float f = cfg_getfloat(cfg_meshframe, "s");
			if (f != 1.0) {
				mf->sx = mf->sy = mf->sz = f;
			}
		}
		
		mf->do_angle = (cfg_getint(cfg_meshframe, "do-angle") == 1 ? true : false);
		
		k = cfg_size(cfg_meshframe, "emission");
		if (k == 4) {
			mf->emission[0] = cfg_getnint(cfg_meshframe, "emission", 0) / 255.0;
			mf->emission[1] = cfg_getnint(cfg_meshframe, "emission", 1) / 255.0;
			mf->emission[2] = cfg_getnint(cfg_meshframe, "emission", 2) / 255.0;
			mf->emission[3] = cfg_getnint(cfg_meshframe, "emission", 3) / 255.0;
		}

		am->meshframes.push_back(mf);
	}
	
	return am;
}


/**
* Constructor
**/
AnimModel::AnimModel()
{
	next = NULL;
}

/**
* Constructor
**/
MeshFrame::MeshFrame()
{
	mesh = NULL;
	texture = NULL;
	emission[0] = emission[1] = emission[2] = emission[3] = 0;
}


/**
* Returns the bounding size of the mesh of the first frame
**/
btVector3 AnimModel::getBoundingSize()
{
	btVector3 ret;
	ret.setX(this->meshframes[0]->mesh->size.x);
	ret.setZ(this->meshframes[0]->mesh->size.y);
	ret.setY(this->meshframes[0]->mesh->size.z);
	return ret;
}


/**
* Returns the bounding size of the mesh of the first frame
* HE = half extents
**/
btVector3 AnimModel::getBoundingSizeHE()
{
	btVector3 ret;
	ret.setX(this->meshframes[0]->mesh->size.x / 2.0f);
	ret.setZ(this->meshframes[0]->mesh->size.y / 2.0f);
	ret.setY(this->meshframes[0]->mesh->size.z / 2.0f);
	return ret;
}



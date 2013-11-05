// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <string>
#include <map>
#include <confuse.h>
#include "../rage.h"
#include "../util/quadratic.h"
#include "confuse_types.h"
#include <glm/glm.hpp>


using namespace std;


/**
* Load a glm::vec3
* The expected config type is CFG_FLOAT_LIST
**/
glm::vec3 cfg_getvec3(cfg_t* cfg, const char* name)
{
	if (cfg_size(cfg, name) == 3) {
		return glm::vec3(
			cfg_getnfloat(cfg, name, 0),
			cfg_getnfloat(cfg, name, 1),
			cfg_getnfloat(cfg, name, 2)
		);
	} else {
		return glm::vec3();
	}
}


/**
* Load a Quadratic
* The expected config type is CFG_FLOAT_LIST
**/
Quadratic cfg_getquadratic(cfg_t* cfg, const char* name)
{
	if (cfg_size(cfg, name) == 3) {
		return Quadratic(
			(float)cfg_getnfloat(cfg, name, 0),
			(float)cfg_getnfloat(cfg, name, 1),
			(float)cfg_getnfloat(cfg, name, 2)
		);
	} else {
		return Quadratic();
	}
}


/**
* Gets a range for a libconfuse config option.
* A range is a min-max value pair in a list
**/
Range cfg_getrange(cfg_t *cfg, const char * name)
{
	Range res;
	
	if (cfg_size(cfg, name) == 1) {
		res.min = cfg_getnint(cfg, name, 0);
		res.max = res.min;
		
	} else {
		res.min = cfg_getnint(cfg, name, 0);
		res.max = cfg_getnint(cfg, name, 1);
	}
	
	return res;
}
// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include "weather.h"
#include "../rage.h"
#include "../game_state.h"
#include "../util/util.h"
#include "../game_engine.h"
#include "../render/render_3d.h"

using namespace std;

#ifdef USE_SPARK
#include "../spark/SPK.h"


/**
* Set up everything
**/
Weather::Weather(GameState* st, float map_size_x, float map_size_z)
{
	this->st = st;
	this->delta = 0.0f;
	this->random = true;

	// Main zone for emission
	this->sky = SPK::AABox::create(SPK::Vector3D(map_size_x/2.0f, 100.0f, map_size_z/2.0f), SPK::Vector3D(map_size_x, 1.0f, map_size_z));

	// Rain model
	this->rain_model = SPK::Model::create(
		SPK::FLAG_RED | SPK::FLAG_GREEN | SPK::FLAG_BLUE | SPK::FLAG_ALPHA,
		SPK::FLAG_NONE,
		SPK::FLAG_RED | SPK::FLAG_GREEN | SPK::FLAG_BLUE | SPK::FLAG_ALPHA 
	);
	this->rain_model->setParam(SPK::PARAM_ALPHA, 0.2f, 0.3f);
	this->rain_model->setParam(SPK::PARAM_RED, 0.7f, 0.9f);
	this->rain_model->setParam(SPK::PARAM_GREEN, 0.7f, 0.9f);
	this->rain_model->setParam(SPK::PARAM_BLUE, 0.5f, 1.0f);
	this->rain_model->setLifeTime(3.0f, 5.0f);

	// Rain emitter
	this->rain_emitter = SPK::StraightEmitter::create(SPK::Vector3D(0.0f, -1.0f, 0.0f));
	this->rain_emitter->setZone(this->sky);
	this->rain_emitter->setTank(-1);
	this->rain_emitter->setForce(40.0f, 60.0f);

	// Rain group
	this->rain_group = SPK::Group::create(this->rain_model, 50000);
	this->rain_group->setRenderer(static_cast<Render3D*>(GEng()->render)->renderer_lines);
	this->rain_group->addEmitter(this->rain_emitter);
}


/**
* Clean up stuff
**/
Weather::~Weather()
{
	delete this->rain_model;
	delete this->rain_group;
	delete this->rain_emitter;
	delete this->sky;
}


/**
* If enough time has passed, we re-calc the weather
**/
void Weather::update(float delta)
{
	if (! this->random) return;
	this->delta += delta;
	if (this->delta >= RANDOM_UPDATE_TIME) {
		this->randomWeather();
		this->delta = 0.0f;
	}
}


/**
* Do something with the weather
**/
void Weather::randomWeather()
{
	int action = getRandom(0, 8);
	if (action <= 5) {
		this->stopRain();
	} else {
		switch (action) {
			case 6: this->startRain(3000); break;
			case 7: this->startRain(5000); break;
			case 8: this->startRain(15000); break;
		}
	}
}


/**
* Start rain falling, or updates existing rain settings
**/
void Weather::startRain(int flow)
{
	this->rain_emitter->setFlow(flow);
	st->removeParticleGroup(this->rain_group);
	st->addParticleGroup(this->rain_group);
}


/**
* Stop rain falling
**/
void Weather::stopRain()
{
	st->removeParticleGroup(this->rain_group);
}


#endif


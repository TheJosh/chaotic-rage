// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include "weather.h"
#include "../rage.h"
#include "../game_state.h"
#include "../util/util.h"
#include "../game_engine.h"
#include "../render/render_3d.h"
#include "../spark/SPK.h"

using namespace std;



/**
* Set up everything
**/
Weather::Weather(GameState* st, float map_size_x, float map_size_z, bool enabled)
{
	this->st = st;
	this->delta = 0.0f;
	this->random = true;
	this->enabled = enabled;
	this->rain_flow = 0;
	this->snow_flow = 0;

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
	if (GEng()->render->is3D()) {
		this->rain_group->setRenderer(static_cast<Render3D*>(GEng()->render)->renderer_lines);
	}
	this->rain_group->addEmitter(this->rain_emitter);


	// Snow model
	this->snow_model = SPK::Model::create(
		SPK::FLAG_RED | SPK::FLAG_GREEN | SPK::FLAG_BLUE | SPK::FLAG_ALPHA,
		SPK::FLAG_NONE,
		SPK::FLAG_RED | SPK::FLAG_GREEN | SPK::FLAG_BLUE | SPK::FLAG_ALPHA 
	);
	this->snow_model->setParam(SPK::PARAM_ALPHA, 0.5f, 0.7f);
	this->snow_model->setParam(SPK::PARAM_RED, 0.8f, 0.9f);
	this->snow_model->setParam(SPK::PARAM_GREEN, 0.8f, 0.9f);
	this->snow_model->setParam(SPK::PARAM_BLUE, 0.8f, 0.9f);
	this->snow_model->setLifeTime(3.0f, 5.0f);

	// Snow emitter
	this->snow_emitter = SPK::StraightEmitter::create(SPK::Vector3D(0.0f, -1.0f, 0.0f));
	this->snow_emitter->setZone(this->sky);
	this->snow_emitter->setTank(-1);
	this->snow_emitter->setForce(30.0f, 50.0f);

	// Snow group
	this->snow_group = SPK::Group::create(this->snow_model, 50000);
	this->snow_group->addEmitter(this->snow_emitter);
}


/**
* Clean up stuff
**/
Weather::~Weather()
{
	delete this->rain_model;
	delete this->rain_group;
	delete this->rain_emitter;
	delete this->snow_model;
	delete this->snow_group;
	delete this->snow_emitter;
	delete this->sky;
}


/**
* If enough time has passed, we re-calc the weather
**/
void Weather::update(float delta)
{
	if (!this->enabled || !this->random) return;
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
	int action = getRandom(0, 10);
	if (action <= 5) {
		this->stopRain();
		this->stopSnow();
	} else {
		switch (action) {
			case 6: this->startRain(MAX_RAIN_FLOW / 5); break;
			case 7: this->startRain(MAX_RAIN_FLOW / 3); break;
			case 8: this->startRain(MAX_RAIN_FLOW); break;
			case 9: this->startSnow(MAX_RAIN_FLOW / 2); break;
			case 10: this->startSnow(MAX_RAIN_FLOW); break;
		}
	}
}


/**
* Start rain falling, or updates existing rain settings
**/
void Weather::startRain(int flow)
{
	if (!this->enabled) return;
	this->rain_emitter->setFlow(flow);
	st->removeParticleGroup(this->rain_group);
	st->addParticleGroup(this->rain_group);
	this->rain_flow = flow;
}


/**
* Stop rain falling
**/
void Weather::stopRain()
{
	st->removeParticleGroup(this->rain_group);
	this->rain_flow = 0;
}


/**
* Start snow falling, or updates existing snow settings
**/
void Weather::startSnow(int flow)
{
	if (!this->enabled) return;
	this->snow_emitter->setFlow(flow);
	st->removeParticleGroup(this->snow_group);
	st->addParticleGroup(this->snow_group);
	this->snow_flow = flow;
}


/**
* Stop snow falling
**/
void Weather::stopSnow()
{
	st->removeParticleGroup(this->snow_group);
	this->snow_flow = 0;
}


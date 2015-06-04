// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once

class GameState;

// Class forward references
namespace SPK
{
	class Zone;
	class Emitter;
	class Group;
	class Model;
}


/**
* How often to recalculate the weather
**/
#define RANDOM_UPDATE_TIME 30000.0f

/**
* Maximum particle flow rates
**/
#define MAX_RAIN_FLOW 15000
#define MAX_SNOW_FLOW 15000


/**
* Weather such as rain etc
**/
class Weather
{
	private:
		GameState* st;
		float delta;
		bool enabled;
		bool random;

		/**
		* Where all weather comes from
		**/
		SPK::Zone* sky;

		/**
		* Rain
		**/
		SPK::Emitter* rain_emitter;
		SPK::Group* rain_group;
		SPK::Model* rain_model;
		int rain_flow;

		/**
		* Snow
		**/
		SPK::Emitter* snow_emitter;
		SPK::Group* snow_group;
		SPK::Model* snow_model;
		int snow_flow;

	public:
		Weather(GameState* st, float map_size_x, float map_size_z, bool enabled);
		~Weather();

	public:
		/**
		* Update the internal clock
		**/
		void update(float delta);

		/**
		* The internal clock will do random weather at regular intervals
		**/
		void randomWeather();

		/**
		* Enable or disable the random weather
		**/
		void enableRandom() { this->random = true; }
		void disableRandom() { this->random = false; }

		/**
		* Specific control for rain
		**/
		void startRain(int flow);
		void stopRain();
		float getRainFlow() {
			return (static_cast<float>(this->rain_flow) / static_cast<float>(MAX_RAIN_FLOW));
		}

		/**
		* Specific control for snow
		**/
		void startSnow(int flow);
		void stopSnow();
		float getSnowFlow() {
			return (static_cast<float>(this->snow_flow) / static_cast<float>(MAX_SNOW_FLOW));
		}
};

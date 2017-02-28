// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <LinearMath/btVector3.h>


class btVector3;
class GameState;
class Mod;
class Sprite;

namespace SPK {
	class Model;
	class Renderer;
	namespace GL {
		class GL2InstanceQuadRenderer;
	}
}


class EffectsManager {
	public:
		EffectsManager(GameState* st);
		~EffectsManager();

		void setUpCoreEffects();

		void weaponBullets(btVector3 * begin, btVector3 * end);
		void weaponFlamethrower(btVector3 * begin, btVector3 * end);
		void bloodSpray(const btVector3& location, float damage);
		void explosion(const btVector3& location, float damage);

	private:
		GameState* st;

		SPK::GL::GL2InstanceQuadRenderer* points;

		SPK::Model* bullets;
		SPK::Model* flames;
		SPK::Model* blood;
		SPK::Model* fireball;
		SPK::Model* dust;
};

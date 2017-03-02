// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <LinearMath/btVector3.h>


class btVector3;
class GameState;
class Mod;
class Sprite;
class Texture2DArray;

namespace SPK {
	class Model;
	class Renderer;
	namespace GL {
		class GL2InstanceQuadRenderer;
		class GL2ColorQuadRenderer;
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

		SPK::GL::GL2ColorQuadRenderer* points;

		SPK::Model* bullets;
		SPK::Model* flames;
		SPK::Model* dust;

		Texture2DArray* tex_fireball;
		SPK::GL::GL2InstanceQuadRenderer* render_fireball;
		SPK::Model* model_fireball;

		Sprite* tex_blood;
		SPK::GL::GL2ColorQuadRenderer* render_blood;
		SPK::Model* model_blood;
};

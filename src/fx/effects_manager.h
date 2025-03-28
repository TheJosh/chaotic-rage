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
	class Group;
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

		void weaponBullets(btVector3* begin, btVector3* end, float speed);
		void weaponFlamethrower(btVector3 * begin, btVector3 * end);
		void bloodSpray(const btVector3& location, float damage);
		void explosion(const btVector3& location, float damage);
		void smokeCloud(const btVector3& location);

	private:
		GameState* st;

		SPK::GL::GL2ColorQuadRenderer* render_bullets;
		SPK::Model* bullets_model;
		SPK::Group* bullets_group;

		Texture2DArray* tex_flames;
		SPK::GL::GL2InstanceQuadRenderer* render_flames;
		SPK::Model* flames_model;
		SPK::Group* flames_group;

		Texture2DArray* tex_smoke;
		SPK::GL::GL2InstanceQuadRenderer* render_smoke;
		SPK::Model* model_smoke;

		Texture2DArray* tex_fireball;
		SPK::GL::GL2InstanceQuadRenderer* render_fireball;
		SPK::Model* model_fireball;

		Texture2DArray* tex_blood;
		SPK::GL::GL2InstanceQuadRenderer* render_blood;
		SPK::Model* model_blood;
};

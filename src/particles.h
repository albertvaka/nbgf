#pragma once

#include "partsys.h"
#include "anim_lib.h"
#include "assets.h"

extern float mainClock;

struct Particles
{
	static inline PartSys explosion = PartSys(nullptr);

	static void InitParticles() {
		if (explosion.texture != nullptr) {
			return; // It's already been init
		}
		explosion.SetTexture(Assets::spritesTexture);
		explosion.AddSprite(AnimLib::BULLET);
		explosion.min_scale = 0.3f;
		explosion.max_scale = 0.8f;
		explosion.max_vel = vec(50, 50);
		explosion.min_vel = vec(-50, -50);
		explosion.min_rotation = 0;
		explosion.max_rotation = 360;
		explosion.rotation_vel = 100.f;
		explosion.scale_vel = -0.2f;
	}
};

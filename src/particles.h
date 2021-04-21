#pragma once

#include "partsys.h"
#include "anim_lib.h"
#include "assets.h"

extern float mainClock;

struct Particles
{
	static inline PartSys explosion = PartSys(nullptr);
	static inline PartSys playerExplosion = PartSys(nullptr);

	static void InitParticles() {
		if (explosion.texture != nullptr) {
			return; // It's already been init
		}
		GPU_Rect alienSprite = AnimLib::ALIEN_BIG;
		explosion.SetTexture(Assets::spritesTexture);
		explosion.AddSprite({ alienSprite.x, alienSprite.y, 8, 8 });
		explosion.AddSprite({ alienSprite.x+8, alienSprite.y, 8, 8 });
		explosion.AddSprite({ alienSprite.x, alienSprite.y+8, 8, 8 });
		explosion.AddSprite({ alienSprite.x+8, alienSprite.y+8, 8, 8 });
		explosion.min_scale = 0.5f;
		explosion.max_scale = 0.9f;
		explosion.max_vel = vec(50, 50);
		explosion.min_vel = vec(-50, -50);
		explosion.min_rotation = 0;
		explosion.max_rotation = 360;
		explosion.rotation_vel = 100.f;
		explosion.scale_vel = -0.2f;

		GPU_Rect playerSprite = AnimLib::PLAYER;
		playerExplosion.SetTexture(Assets::spritesTexture);
		playerExplosion.AddSprite({ playerSprite.x, playerSprite.y, 8, 8 });
		playerExplosion.AddSprite({ playerSprite.x + 8, playerSprite.y, 8, 8 });
		playerExplosion.AddSprite({ playerSprite.x, playerSprite.y + 8, 8, 8 });
		playerExplosion.AddSprite({ playerSprite.x + 8, playerSprite.y + 8, 8, 8 });
		playerExplosion.min_scale = 1.0f;
		playerExplosion.max_scale = 1.2f;
		playerExplosion.max_vel = vec(50, 50);
		playerExplosion.min_vel = vec(-50, -50);
		playerExplosion.min_rotation = 0;
		playerExplosion.max_rotation = 360;
		playerExplosion.rotation_vel = 100.f;
		playerExplosion.scale_vel = -0.15f;
	}
};

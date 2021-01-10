#pragma once

#include "../engine/anim_frame.h"

#include "texturepackerexport.h"

namespace AnimLib {

	constexpr const AnimationFrame FIRESLIME_WALK[] = {
		{ { 13 * 16.f, 0 * 16.f, 16.f, 16.f }, .1f },
		{ { 14 * 16.f, 0 * 16.f, 16.f, 16.f }, .1f },
		{ { 15 * 16.f, 0 * 16.f, 16.f, 16.f }, .1f },
		{ { 16 * 16.f, 0 * 16.f, 16.f, 16.f }, .1f },
		{ { 17 * 16.f, 0 * 16.f, 16.f, 16.f }, .1f },
		{ { 18 * 16.f, 0 * 16.f, 16.f, 16.f }, .1f },
		{ { 12 * 16.f, 0 * 16.f, 16.f, 16.f }, .1f },
	};

	constexpr const AnimationFrame FIRESLIME_ATTACK[] = {
		{ { 20 * 16.f, 0 * 16.f, 16.f, 16.f }, .2f },
		{ { 21 * 16.f, 0 * 16.f, 16.f, 16.f }, .1f },
		{ { 22 * 16.f, 0 * 16.f, 16.f, 16.f }, .4f },
		{ { 25 * 16.f, 0 * 16.f, 16.f, 16.f }, .1f },
		{ { 26 * 16.f, 0 * 16.f, 16.f, 16.f }, .1f },
		{ { 12 * 16.f, 0 * 16.f, 16.f, 16.f }, .1f },
	};

	constexpr const AnimationFrame MAGIC_EXPLOSION[] = {
		{ {  9 * 16.f, 10 * 16.f, 16.f, 16.f }, .05f },
		{ { 10 * 16.f, 10 * 16.f, 16.f, 16.f }, .05f },
		{ { 11 * 16.f, 10 * 16.f, 16.f, 16.f }, .05f },
		{ { 12 * 16.f, 10 * 16.f, 16.f, 16.f }, .05f },
		{ { 13 * 16.f, 10 * 16.f, 16.f, 16.f }, .05f },
		{ { 14 * 16.f, 10 * 16.f, 16.f, 16.f }, .05f },
		{ { 15 * 16.f, 10 * 16.f, 16.f, 16.f }, .05f },
	};

	constexpr const AnimationFrame MANTIS_PREPARE_JUMP[] = {
		{ {2 * 32, 8 * 32, 32, 32}, .17f },
		{ {1 * 32, 8 * 32, 32, 32}, .1f },
		{ {0 * 32, 8 * 32, 32, 32}, .1f },
	};

	constexpr const GPU_Rect MANTIS_AIR = { 1 * 32, 8 * 32, 32, 32 };

	constexpr const AnimationFrame MANTIS_WALK[] = {
		{ {0 * 32, 10 * 32, 32, 32}, .1f },
		{ {1 * 32, 10 * 32, 32, 32}, .1f },
		{ {2 * 32, 10 * 32, 32, 32}, .1f },
		{ {3 * 32, 10 * 32, 32, 32}, .1f },
	};

	constexpr const AnimationFrame GOOMBA[] = {
		{ {14 * 16, 16, 16, 16}, .2f },
		{ {15 * 16, 16, 16, 16}, .2f },
	};

	constexpr const AnimationFrame GOOMBACHARGER[] = {
		{ {16 * 16, 16, 16, 16}, .2f },
		{ {17 * 16, 16, 16, 16}, .2f },
	};
	constexpr const AnimationFrame BIPEDAL_WALKING[] = {
		{ TexturePackerRects::BIPEDALUNIT_WALKING_001_000, .25f },
		{ TexturePackerRects::BIPEDALUNIT_WALKING_001_001, .25f },
		{ TexturePackerRects::BIPEDALUNIT_WALKING_001_002, .25f },
		{ TexturePackerRects::BIPEDALUNIT_WALKING_001_003, .25f },
		{ TexturePackerRects::BIPEDALUNIT_WALKING_001_004, .25f },
		{ TexturePackerRects::BIPEDALUNIT_WALKING_001_005, .25f },
	};

	constexpr const AnimationFrame MISSILE[] = {
		{ TexturePackerRects::BOSSWHEELERXMISSILE_DEFAULT_000, 0.1f },
		{ TexturePackerRects::BOSSWHEELERXMISSILE_DEFAULT_001, 0.1f },
	};

	constexpr const AnimationFrame EXPLOSION[] = {
		{ TexturePackerRects::EXPLOSION_01, 0.05f },
		{ TexturePackerRects::EXPLOSION_02, 0.05f },
		{ TexturePackerRects::EXPLOSION_03, 0.05f },
		{ TexturePackerRects::EXPLOSION_04, 0.05f },
		{ TexturePackerRects::EXPLOSION_05, 0.05f },
		{ TexturePackerRects::EXPLOSION_06, 0.05f },
		{ TexturePackerRects::EXPLOSION_07, 0.05f },
		{ TexturePackerRects::EXPLOSION_08, 0.05f },
		{ TexturePackerRects::EXPLOSION_09, 0.05f },
	};

	constexpr const AnimationFrame FLYING_ALIEN[] = {
		{ TexturePackerRects::FLYINGALIEN_01, 0.05f },
		{ TexturePackerRects::FLYINGALIEN_02, 0.05f },
		{ TexturePackerRects::FLYINGALIEN_03, 0.05f },
		{ TexturePackerRects::FLYINGALIEN_04, 0.05f },
		{ TexturePackerRects::FLYINGALIEN_05, 0.05f },
		{ TexturePackerRects::FLYINGALIEN_06, 0.05f },
		{ TexturePackerRects::FLYINGALIEN_07, 0.05f },
		{ TexturePackerRects::FLYINGALIEN_08, 0.05f },
	};

	constexpr const AnimationFrame HIT_SPLASH[] = {
		{ { 7 * 32, 8 * 32, 40, 32 }, 0.05f },
	};

	constexpr const GPU_Rect BIPEDAL_IDLE = TexturePackerRects::BIPEDALUNIT_IDLE_00;
	
	constexpr const GPU_Rect FIRESHOT = {437, 5, 6, 6};

	constexpr const GPU_Rect HEALTH_FULL = { 19 * 16, 1 * 16, 16, 16 };
	constexpr const GPU_Rect HEALTH_EMPTY = { 20 * 16, 1 * 16, 16, 16 };

}

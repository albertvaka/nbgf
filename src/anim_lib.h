#pragma once

#include "../engine/anim_frame.h"

#include "texturepackerexport.h"

namespace AnimLib {


	constexpr const AnimationFrame MAGIC_EXPLOSION[] = {
		{ {  9 * 16.f, 10 * 16.f, 16.f, 16.f }, .05f },
		{ { 10 * 16.f, 10 * 16.f, 16.f, 16.f }, .05f },
		{ { 11 * 16.f, 10 * 16.f, 16.f, 16.f }, .05f },
		{ { 12 * 16.f, 10 * 16.f, 16.f, 16.f }, .05f },
		{ { 13 * 16.f, 10 * 16.f, 16.f, 16.f }, .05f },
		{ { 14 * 16.f, 10 * 16.f, 16.f, 16.f }, .05f },
		{ { 15 * 16.f, 10 * 16.f, 16.f, 16.f }, .05f },
	};

	constexpr const AnimationFrame GOOMBA[] = {
		{ {12 * 16, 0, 16,16}, .2f },
		{ {13 * 16, 0, 16,16}, .2f },
	};

	constexpr const AnimationFrame GOOMBACHARGER[] = {
		{ {14 * 16, 0, 16,16}, .2f },
		{ {15 * 16, 0, 16,16}, .2f },
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

	constexpr const GPU_Rect BIPEDAL_IDLE = TexturePackerRects::BIPEDALUNIT_IDLE_00;

}

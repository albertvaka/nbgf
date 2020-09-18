#pragma once

#include "../engine/anim_frame.h"

#include "texturepackerexport.h"

namespace AnimLib {

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

	constexpr const GPU_Rect BIPEDAL_IDLE = TexturePackerRects::BIPEDALUNIT_IDLE_00;

}

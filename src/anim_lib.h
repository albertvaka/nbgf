#pragma once

#include "../engine/anim_frame.h"

#include "texturepackerexport.h"

namespace AnimLib {

	constexpr const AnimationFrame BIPEDAL_WALKING[] = {
		{ TexturePackerRects::BIPEDALUNIT_WALKING_001_000, .2f },
		{ TexturePackerRects::BIPEDALUNIT_WALKING_001_001, .2f },
		{ TexturePackerRects::BIPEDALUNIT_WALKING_001_002, .2f },
		{ TexturePackerRects::BIPEDALUNIT_WALKING_001_003, .2f },
		{ TexturePackerRects::BIPEDALUNIT_WALKING_001_004, .2f },
		{ TexturePackerRects::BIPEDALUNIT_WALKING_001_005, .2f },
	};

	constexpr const AnimationFrame MISSILE[] = {
		{ TexturePackerRects::BOSSWHEELERXMISSILE_DEFAULT_000, 0.1f },
		{ TexturePackerRects::BOSSWHEELERXMISSILE_DEFAULT_001, 0.1f },
	};

	constexpr const GPU_Rect BIPEDAL_IDLE = TexturePackerRects::BIPEDALUNIT_IDLE_00;

}

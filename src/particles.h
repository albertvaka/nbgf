#pragma once

#include <array>

#include "partsys.h"

namespace Particles
{
	inline PartSys waterTrail(nullptr);

	void Init();
	void UpdateAll(float dt);
	void ClearAll();
};

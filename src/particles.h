#pragma once

#include <array>

#include "partsys.h"

namespace Particles
{
	inline PartSys dust(nullptr);
	inline PartSys bullet(nullptr);
	inline PartSys health(nullptr);
	inline PartSys missile(nullptr);
	inline PartSys itemSparks(nullptr);
	inline PartSys ooyTearTrail(nullptr);
	inline PartSys leafs(nullptr);

	void Init();
	void UpdateAll(float dt);
	void ClearAll();

	void DoDustJump(vec pos);
	void DoDustWallJump(vec pos, bool toTheLeft);
	void DoDustLand(vec pos);
	void DoDustRun(vec pos, float dt, bool toTheLeft, bool moarDust);

};


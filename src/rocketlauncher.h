#pragma once

#include "vec.h"
#include "bounds.h"
#include "selfregister.h"

#include <vector>

struct RocketLauncher : SelfRegister<RocketLauncher>
{
	RocketLauncher(vec position);

	void Update(float dt);
	void Draw() const;

	vec pos;
	float angle = 0;
	float timer = 0.f;
	std::vector<BoxBounds> awakeArea;

};


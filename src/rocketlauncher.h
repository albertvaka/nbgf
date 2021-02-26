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
	bool awoken = false;
	float angle = 90.f;
	float timer = 0.f;
	std::vector<BoxBounds> awakeArea;

};


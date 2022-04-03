#pragma once

#include "vec.h"
#include "selfregister.h"

struct Drain : SelfRegister<Drain>
{
	Drain(vec position);

	void Update(float dt);
	void Draw() const;

	vec pos;
	int screen;
	float timer;
};


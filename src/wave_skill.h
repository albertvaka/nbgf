#pragma once

#include "entity.h"
#include "selfregister.h"
#include "assets.h"
#include "person.h"
#include "window.h"
#include "text.h"

static const float kMaxRadius = 2000;
static const float kSpeed = 250;

struct WaveSkill : SelfRegister<WaveSkill>
{
	float radius = 1;
	bool alive = true;
	vec pos;

	WaveSkill(vec pos) : pos(pos)
	{
	}

	bool InWave(vec p) {
		float dist = p.Distance(pos);
		constexpr float radiusOffset = 20.f;
		return (dist < radius + radiusOffset && dist > radius - radiusOffset);
	}

	void Update(float dt)
	{
		radius += kSpeed*dt;
		if (radius > kMaxRadius) alive = false;
	}
	
	void Draw() const
	{
		Window::DrawPrimitive::Circle(pos, radius, 5, 255, 255, 0);
	}
};
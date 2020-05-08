#pragma once

#include "partsys.h"
#include "selfregister.h"
#include "bounds.h"

struct Lava : EntS<Lava>
{
	PartSys lavaPartSys;

	Bounds bounds;
	float targetY;

	Lava(const Bounds& bounds);
	void Update(float dt);
	void Draw() const;

	void RaiseTo(float newY) {
		targetY = newY;
	}

	void Plof(float posX) {
		lavaPartSys.pos.x = posX;
		for (int i = 0; i < 12; i++) {
			auto& lavaP = lavaPartSys.AddParticle();
			lavaP.vel.y *= 1.25f;
		}
	}

	bool IsInside(const vec& pos) const {
		return bounds.Contains(pos);
	}

};


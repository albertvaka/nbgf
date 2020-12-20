#pragma once

#include "partsys.h"
#include "selfregister.h"
#include "bounds.h"

struct Lava : SelfRegister<Lava>
{
	PartSys lavaPartSys;

	Bounds bounds;
	float targetY;

	explicit Lava(const Bounds& bounds);
	void Update(float dt);
	void Draw() const;

	void SetLevel(float newY, bool immediate = false) {
		targetY = newY;
		if (immediate) {
			bounds.height += bounds.top - targetY;
			bounds.top = targetY;
		}
	}

	float CurrentLevel() {
		return bounds.top;
	}

	void Plof(float posX) {
		lavaPartSys.pos.x = posX;
		for (int i = 0; i < 12; i++) {
			auto& lavaP = lavaPartSys.AddParticle();
			lavaP.vel.y *= 1.25f;
		}
	}

	bool IsInside(vec pos) const {
		return bounds.Contains(pos);
	}

};


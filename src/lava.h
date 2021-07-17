#pragma once

#include "partsys.h"
#include "selfregister.h"
#include "bounds.h"

struct Lava : SelfRegister<Lava>
{
	static constexpr const float kRaiseSpeed = 15.f;
	static constexpr const float kFastRaiseSpeed = 30.f;

	PartSys lavaPartSys;

	BoxBounds bounds;
	float targetY;

	float raiseSpeed = kRaiseSpeed;

	float timer = 0.f;
	explicit Lava(const BoxBounds& bounds);
	void Update(float dt);
	void Draw() const;

	void SetLevel(float newY, bool immediate = false);

	void SetRaiseSpeed(float speed) {
		raiseSpeed = speed;
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

	Mates::Range GetChunksOnScreen() const;

};


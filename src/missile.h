#pragma once

#include "entity.h"
#include "animation.h"
#include "anim_lib.h"
#include "partsys.h"
#include "selfregister.h"

struct Missile : CircleEntity, SelfRegister<Missile>
{
	bool exploding = false;
	Animation anim;
	float smokeTimer = 0;

	static inline PartSys particles = PartSys(nullptr);
	static void InitParticles();

	Missile(vec position, float angleDegs);

	void explode() {
		exploding = true;
		anim.Set(AnimLib::EXPLOSION);
		anim.loopable = false;
	}

	void Update(float dt);

	void Draw() const;
};

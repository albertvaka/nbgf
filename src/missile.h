#pragma once

#include "entity.h"
#include "animation2.h"
#include "anim_lib.h"
#include "partsys.h"
#include "selfregister.h"

struct Missile : CircleEntity, SelfRegister<Missile>
{
	bool explode = false;
	Animation2 anim;
	float smokeTimer = 0;

	static inline PartSys particles = PartSys(nullptr);
	static void InitParticles();

	Missile(const vec& position, float angleDegs);

	void boom() {
		explode = true;
		anim.SetAnimation(AnimLib::EXPLOSION);
		anim.loopable = false;
	}

	void Update(float dt);

	void Draw() const;
};

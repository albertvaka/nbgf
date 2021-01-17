#pragma once

#include "vec.h"
#include "entity.h"
#include "selfregister.h"
#include "partsys.h"

struct Health : BoxEntity, SelfRegister<Health>
{
	Health(vec p) : BoxEntity(p, vec(10,10)) { }
	void Update(float dt);
	void Draw() const;

	static inline PartSys particles = PartSys(nullptr);
	static void InitParticles();
	float particlesTimer = 0.f;
	vec drawOffset() const;
};

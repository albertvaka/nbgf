#pragma once

#include "vec.h"
#include "entity.h"
#include "selfregister.h"

struct Health : BoxEntity, SelfRegister<Health>
{
	Health(vec p) : BoxEntity(p, vec(10,10)) { }
	void Update(float dt);
	void Draw() const;

	float particlesTimer = 0.f;
	vec drawOffset() const;
};

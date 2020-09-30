#pragma once

#include "entity.h"
#include "partsys.h"
#include "selfregister.h"

struct Bullet : CircleEntity, SelfRegister<Bullet>
{
	float scale;

	static inline PartSys particles = PartSys(nullptr);
	static void InitParticles();

	Bullet(const vec& position, const vec& velocity, float _scale = 1.f) {
		pos = position;
		vel = velocity;
		radius = 5 * _scale;
		scale = _scale;
	}

	void explode();

	void Update(float dt);
	void Draw() const;

};

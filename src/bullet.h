#pragma once

#include "entity.h"
#include "partsys.h"
#include "selfregister.h"

struct Bullet : CircleEntity, SelfRegister<Bullet>
{
	bool explode = false;
	float timer_explosion = 0;
	float scale;

	static inline PartSys particles = PartSys(nullptr);
	static void InitParticles();

	Bullet(const vec& position, const vec& velocity, float _scale = 1.f) {
		pos = position;
		vel = velocity;
		radius = 5 * _scale;
		scale = _scale;
	}

	void boom(const vec& explosionPos) {
		pos = explosionPos;
		boom();
	}
	void boom();

	void Update(float dt);
	void Draw() const;

};

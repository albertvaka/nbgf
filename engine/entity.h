#pragma once

#include "vec.h"
#include "bounds.h"

struct Entity
{
	constexpr Entity() : pos(0.f,0.f) {}
	constexpr Entity(vec pos) : pos(pos) {}
	constexpr Entity(vec pos, vec vel) : pos(pos), vel(vel) {}
	vec pos;
	vec vel = vec(0.f, 0.f);
	bool alive = true;
};

struct BoxEntity : Entity {
	constexpr BoxEntity(vec size) : size(size) {}
	constexpr BoxEntity(vec pos, vec size) : Entity(pos), size(size) {}
	constexpr BoxEntity(const BoxBounds& b) : Entity(b.Center()), size(b.Size()) {} //note that entities position is on their center

	vec size;

	[[nodiscard]] constexpr BoxBounds Bounds() const {
		return BoxBounds::FromCenter(pos, size);
	}
};

struct CircleEntity : Entity {
	constexpr CircleEntity() : radius(8.f) {}
	constexpr CircleEntity(float radius) : radius(radius) {}
	constexpr CircleEntity(vec pos, float radius) : Entity(pos), radius(radius) {}
	constexpr CircleEntity(vec pos, float radius, vec vel) : Entity(pos, vel), radius(radius) {}

	float radius;
	
	[[nodiscard]] constexpr CircleBounds Bounds() const {
		return CircleBounds(pos, radius);
	}
};



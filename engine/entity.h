#pragma once

#include "vec.h"
#include "bounds.h"

struct FromTheFutureEntity
{
	constexpr FromTheFutureEntity() : pos(0.f,0.f) {}
	constexpr FromTheFutureEntity(vec pos) : pos(pos) {}
	constexpr FromTheFutureEntity(vec pos, vec vel) : pos(pos), vel(vel) {}
	vec pos;
	vec vel = vec(0.f, 0.f);
	bool alive = true;
};

struct BoxEntity : FromTheFutureEntity {
	constexpr BoxEntity(vec size) : size(size) {}
	constexpr BoxEntity(vec pos, vec size) : FromTheFutureEntity(pos), size(size) {}
	constexpr BoxEntity(const BoxBounds& b) : FromTheFutureEntity(b.Center()), size(b.Size()) {} //note that entities position is on their center

	vec size;

	[[nodiscard]] constexpr BoxBounds Bounds() const {
		return BoxBounds::FromCenter(pos, size);
	}
};

struct CircleEntity : FromTheFutureEntity {
	constexpr CircleEntity() : radius(8.f) {}
	constexpr CircleEntity(float radius) : radius(radius) {}
	constexpr CircleEntity(vec pos, float radius) : FromTheFutureEntity(pos), radius(radius) {}
	constexpr CircleEntity(vec pos, float radius, vec vel) : FromTheFutureEntity(pos, vel), radius(radius) {}

	float radius;
	
	[[nodiscard]] constexpr CircleBounds Bounds() const {
		return CircleBounds(pos, radius);
	}
};



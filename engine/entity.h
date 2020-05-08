#pragma once

#include "vector.h"
#include "bounds.h"

struct Entity
{
	constexpr Entity() : pos (0.f,0.f) {}
	constexpr Entity(const vec& pos) : pos(pos) {}
	vec pos;
	vec vel = vec(0.f,0.f);
	bool alive = true;
};

struct BoxEntity : Entity {
	constexpr BoxEntity(const vec& pos, const vec& size) : Entity(pos), size(size) {}
	constexpr BoxEntity(const Bounds& b) : Entity(b.Center()), size(b.Size()) {} //note that entities position is on their center

	vec size;

	constexpr Bounds bounds() const {
		return Bounds::fromCenter(pos, size);
	}

	void drawBounds(uint8_t r = 255, uint8_t g = 255, uint8_t b = 255) const {
		bounds().Draw(r,g,b);
	}
};

struct CircleEntity : Entity {
	constexpr CircleEntity() : radius(8.f) {}
	constexpr CircleEntity(const vec& pos, float radius) : Entity(pos), radius(radius) {}

	float radius;
	
	constexpr CircleBounds bounds() const {
		return CircleBounds(pos, radius);
	}

	void drawBounds(uint8_t r = 255, uint8_t g = 255, uint8_t b = 255) const {
		bounds().Draw(r,g,b);
	}
};



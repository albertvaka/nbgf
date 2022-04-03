#pragma once

#include "vec.h"
#include "animation.h"
#include "selfregister.h"
#include "entity.h"

struct Bipedal : Entity, SelfRegister<Bipedal>
{
	enum class State {
		DEACTIVATED,
		WALKING_FORWARD,
		WALKING_BACK,
		DRAMATIC_PAUSE,
		FIRING,
		DYING,
	};

	Animation anim;
	State state;
	int screen;
	float minX, maxX;
	bool charging = false;
	float timer = 0.f;
	float hitTimer = 0.f;
	int health;
	BoxBounds legsHitBox, headHitBox;

	Bipedal(vec position);

	void Update(float dt);
	void Draw() const;
	void TakeDamage();
	void Die();

	/*
	template<typename T>
	bool Collides(const T& t) {
		return Collide(t, headHitBox) || Collide(t, legsHitBox);
	}*/

};


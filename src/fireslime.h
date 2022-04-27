#pragma once

#include "vec.h"
#include "selfregister.h"
#include "animation.h"
#include "entity.h"

struct FireSlime : CircleEntity, SelfRegister<FireSlime>
{
	enum class State
	{
		WALKING,
		ATTACKING,
	};

	State state = State::WALKING;
	float timer = 0.0f;
	Animation anim;

	float direction; // -1 or 1

	int screen;

	bool willAttack = false; // will attack after landing
	bool didJustAttack = false; // to force one move between attacks

	FireSlime(vec position);
	~FireSlime();

	BoxBounds AttackBounds() const;
	bool CanMoveForward() const;

	void Update(float dt);
	void Draw() const;

};


#pragma once

#include "vec.h"
#include "selfregister.h"
#include "animation2.h"
#include "anim_lib.h"
#include "entity.h"
#include "oneshotanim.h"

struct FireSlime : CircleEntity, SelfRegister<FireSlime>
{
	enum class State
	{
		WALKING,
		ATTACKING,
	};

	State state = State::WALKING;
	float timer = 0.0f;
	Animation2 anim;

	bool willAttack = false; // will attack after landing
	bool didJustAttack = false; // to force one move between attacks

	float direction; // -1 or 1

	FireSlime(const vec& position);

	Bounds AttackBounds() const;
	bool CanMoveForward() const;

	void Update(float dt);
	void Draw() const;

	void die() {
		alive = false;
		new OneShotAnim(pos, AnimLib::MAGIC_EXPLOSION, 1.3f);
	}
};


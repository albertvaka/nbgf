#pragma once

#include "vec.h"
#include "selfregister.h"
#include "animation.h"
#include "entity.h"

struct Minotaur : BoxEntity, SelfRegister<Minotaur>
{
	enum class State
	{
		IDLE,
		RUN,
		TAUNT,
		FLIP,
		ATTACK_BIG,
		//ATTACK_SMALL,
		//TAKE_DAMAGE,
		//DIE,
	};

	State state;
	float hitTimer = 0.f;

	Animation anim;

	int screen;
	int health;
	vec initialPos;

	bool goingRight = true;

	Minotaur(vec position);
	~Minotaur();

	void Reset();
	BoxBounds AttackBounds() const;
	BoxBounds FlipAttackBounds() const;

	void Update(float dt);
	void Draw() const;
	int DrawHealth(int offset) const;

	void TakeDamage();
};


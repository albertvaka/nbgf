#pragma once

#include "vec.h"
#include "selfregister.h"
#include "animation.h"
#include "entity.h"

struct Goomba : CircleEntity, SelfRegister<Goomba>
{
	enum class State
	{
		WALKING,
		ENTER_CHARGE,
		EXIT_CHARGE,
		CHARGING,
		SHIELDING,
		TEST_DUMMY
	};

	enum class Type
	{
		DUMMY,
		WALKER,
		CHARGER,
		SHIELDER,
	};

	State state;
	Type type;

	bool goingRight = true;

	float timer = 0.0f;

	Animation anim;

	int screen;
	int hitShieldSoundChannel = -1;

	Goomba(vec position, Type type);
	~Goomba();

	BoxBounds ChargeBounds() const;

	float WalkSpeed() const;
	float WalkDirection() const { return (goingRight ? 1 : -1); }
	float GetVel() { return WalkSpeed() * WalkDirection(); }

	void Walk(float dt);

	void Update(float dt);
	void Draw() const;

};


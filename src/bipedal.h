#pragma once

#include "vec.h"
#include "animation2.h"
#include "selfregister.h"
#include "entity.h"
#include "collide.h"

struct JumpMan;
struct TileMap;

struct Bipedal : Entity, SelfRegister<Bipedal>
{
	enum class State {
		ASLEEP,
		WALKING_FORWARD,
		WALKING_BACK,
		DRAMATIC_PAUSE,
		FIRING,
	};

	Animation2 anim;
	State state;
	int screen;
	float minX, maxX;
	bool charging = false;
	float timer = 0.f;
	float damagedTimer = 0.f;
	Bounds legsHitBox, headHitBox;

	Bipedal(const vec& position);

	void Update(float dt);
	void Draw() const;
	void takeDamage() {
		damagedTimer = 0.3f;
	}
	
	/*
	template<typename T>
	bool Collides(const T& t) {
		return Collide(t, headHitBox) || Collide(t, legsHitBox);
	}*/

	inline bool inSameScreenAsPlayer() const;
};


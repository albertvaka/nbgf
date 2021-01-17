#pragma once

#include "vec.h"
#include "animation2.h"
#include "selfregister.h"
#include "entity.h"

struct JumpMan;
struct TileMap;

struct Bipedal : Entity, SelfRegister<Bipedal>
{
	enum class State {
		DEACTIVATED,
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
	int health = 18;
	Bounds legsHitBox, headHitBox;

	Bipedal(vec position);

	void Update(float dt);
	void Draw() const;
	void takeDamage();
	void die();

	/*
	template<typename T>
	bool Collides(const T& t) {
		return Collide(t, headHitBox) || Collide(t, legsHitBox);
	}*/

};


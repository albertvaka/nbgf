#pragma once

#include "vec.h"
#include "animation2.h"
#include "selfregister.h"
#include "entity.h"

struct JumpMan;
struct TileMap;

struct Bipedal : BoxEntity, SelfRegister<Bipedal>
{
	enum class State {
		IDLE,
		WALKING_FORWARD,
		WALKING_BACK,
		DRAMATIC_PAUSE,
		FIRING,
	};

	Animation2 anim;
	State state;
	int screen;
	float initialX;
	bool charging = false;
	float timer = 0.f;

	Bipedal(const vec& position);

	void Update(float dt);
	void Draw() const;

	inline bool inSameScreenAsPlayer() const;
};


#pragma once

#include "vector.h"
#include "animation.h"
#include "selfregister.h"
#include "entity.h"

class JumpMan;

struct Bat : CircleEntity, EntS<Bat>
{
	enum class State {
		SIESTA,
		FLYING,
	};

	State state = State::FLYING;
	bool explode = false;
	int timer_explosion = 0;
	float timeToAwake = 1000.f; //Does nothing if > 999
	bool awakened = false;

	Animation anim;

	Bat(vec position);

	void Update(JumpMan* jumpman, float dt);
	void Draw(sf::Sprite& spr, sf::RenderTarget& window);

	void DrawSenseArea(sf::RenderTarget& window);
};


#pragma once

#include "vector.h"
#include "animation.h"
#include "selfregister.h"
#include "entity.h"

class JumpMan;

void AwakeNearbyBats(vec pos);

struct Bat : CircleEntity, EntS<Bat>
{
	enum class State {
		SIESTA,
		FLYING,
	};

	Animation anim;
	State state = State::FLYING;
	float timeToAwake = 1000.f; //Does nothing if > 999
	bool awakened = false;

	Bat(vec position);

	void Update(JumpMan* jumpman, float dt);
	void Draw(sf::Sprite& spr, sf::RenderTarget& window);

	void DrawSenseArea(sf::RenderTarget& window);
};


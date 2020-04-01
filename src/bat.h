#pragma once

#include "vector.h"
#include "animation.h"
#include "selfregister.h"
#include "steering_entity.h"
#include "steering_behavior.h"
#include "steering_behavior_applier.h"

struct JumpMan;
struct TileMap;

void AwakeNearbyBats(vec pos);

struct Bat : SteeringEntity, EntS<Bat>
{
	enum class State {
		SIESTA,
		FLYING,
		SEEKING,
	};

	SteeringBehaviorApplier steering;

	Animation anim;
	State state = State::FLYING;
	float timeToAwake = 1000.f; //Does nothing if > 999
	bool awakened = false;
	JumpMan* jumpman;
	bool aggresive = false;

	Bat(vec position, JumpMan* jumpman, TileMap* tilemap);

	void Update(float dt);
	void Draw(sf::RenderTarget& window);

	void DrawSenseArea(sf::RenderTarget& window);
};

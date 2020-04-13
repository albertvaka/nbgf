#pragma once

#include "vector.h"
#include "animation.h"
#include "selfregister.h"
#include "steering_entity.h"
#include "steering_behavior.h"
#include "steering_behavior_applier.h"

struct JumpMan;
struct TileMap;

void AwakeNearbyBats(const vec& pos);

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
	bool aggresive = false;
	float seekingTimer; // Aggresive bats will start seeking when this gets to 0
	int screen;

	Bat(const vec& position, bool aggresive, bool awake);

	void Update(float dt);
	void Draw(sf::RenderTarget& window) const;

	inline bool inSameScreenAsPlayer() const;

	void DrawSenseArea(sf::RenderTarget& window) const;
private:
	void EnableBoundsAvoidance();
};


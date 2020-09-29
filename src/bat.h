#pragma once

#include "vec.h"
#include "animation.h"
#include "selfregister.h"
#include "steering_entity.h"
#include "steering_behavior.h"
#include "steering_behavior_applier.h"
#include "base_enemy.h"

struct Bat : SteeringEntity, SelfRegister<Bat>, BaseEnemy<Bat>
{
	enum class State {
		SIESTA,
		FLYING,
		SEEKING,
	};

	static void AwakeNearbyBats(const vec& pos);

	SteeringBehaviorApplier steering;

	Animation anim;
	State state = State::FLYING;
	float timeToAwake = 1000.f; //Does nothing if > 999
	bool awakened = false;
	bool aggresive = false;
	float seekingTimer; // Aggresive bats will start seeking when this gets to 0
	int screen;

	Bat(const vec& position, bool aggresive, bool awake);

	void takeDamage() {
		alive = false; // one hit
	}

	void Update(float dt);
	void Draw() const;

	inline bool inSameScreenAsPlayer() const;

	void DrawSenseArea() const;
private:
	void EnableBoundsAvoidance();
};


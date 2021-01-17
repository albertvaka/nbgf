#pragma once

#include "vec.h"
#include "animation.h"
#include "selfregister.h"
#include "steering_entity.h"
#include "steering_behavior.h"
#include "steering_behavior_applier.h"

struct Bat : SteeringEntity, SelfRegister<Bat>
{
	enum class State {
		SIESTA,
		AWAKENING,
		FLYING,
		SEEKING,
	};

	static void AwakeNearbyBats(vec pos);

	SteeringBehaviorApplier steering;

	Animation anim;
	State state = State::FLYING;
	float timeToAwake = 1000.f; //Does nothing if > 999
	bool awakened = false;
	bool aggresive = false;
	float seekingTimer; // Aggresive bats will start seeking when this gets to 0
	int screen;

	Bat(vec position, bool aggresive, bool awake);

	void Update(float dt);
	void Draw() const;

private:
	void DrawSenseArea() const;
	void EnableBoundsAvoidance();
};


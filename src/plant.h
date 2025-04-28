#pragma once

#include "entity.h"
#include "selfregister.h"
#include "jumpman.h"

const int kNumTomatoes = 3;

struct Plant : public BoxEntity, SelfRegister<Plant> {
	Plant(vec pos);
	void Update(float dt);
	void Draw() const;

	int id;
	float getBottomY() const;
	bool IsBeingCarried() const;
	bool IsCarriedBy(const JumpMan* carrier) const;
	void PickUpBy(const JumpMan* carrier);
	void Drop();
	bool HasTomato() const;
	void PickTomato();

	// Once hit by water, effect lasts for a bit.
	void SetHitByWater();
	// Is hit by light, until this is set to off.
	void SetHitByLight(bool hit);

	const JumpMan* carrier = nullptr;

	// number of tiles of height, including pot.
	int height = 1;
	bool mirror = false;

	// Stats.
	bool gets_water = false;
	bool gets_light = false;
	float water = 0;
	float light = 0;
	bool has_tomato = false;
	vec tomato_offset[kNumTomatoes];

	float time_left_water_bubble = 0;
	float growClock = 0;

};

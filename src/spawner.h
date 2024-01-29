#pragma once

#include "entity.h"
#include "cadaver.h"
#include "animation.h"
#include "rand.h"
#include "vec.h"

struct Spawner : Entity, SelfRegister<Spawner>
{

	const int INTERVAL = 6000;
	bool empty = true;
	int timer = INTERVAL;
	Spawner(vec position) {
		pos = position;
	}

	void Update(int dt)
	{
		timer += dt;
		if (timer > INTERVAL) {
			if (empty) {
				timer = 0;
				spawn();
			}
		}
		empty = true;
	}

	void spawn() {
		//if (Rand::roll_1d4() > 2) {
			new Cadaver(pos);
		//}
		//else {
		//	new Extremity(pos, RandomExtremityType(), RandomExtremityColor());
		//}
	}

};

struct Detector : Entity, SelfRegister<Detector>
{
	Spawner* spawner;
	Detector(vec position, Spawner *s) {
		pos = position;
		spawner = s;
	}

	void Update(int dt) override
	{

	}

};

struct Despawner : Entity, SelfRegister<Despawner>
{
	Despawner(vec position) {
		pos = position;
	}

};

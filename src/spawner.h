#pragma once

#include "hospital_entity.h"
#include "cadaver.h"
#include "animation.h"
#include "rand.h"
#include "vector.h"

struct Spawner : HospitalEntity, EntS<Spawner>
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
		//if (Random::roll_1d4() > 2) {
			new Cadaver(pos);
		//}
		//else {
		//	new Extremity(pos, RandomExtremityType(), RandomExtremityColor());
		//}
	}

};

struct Detector : HospitalEntity, EntS<Detector>
{
	Spawner* spawner;
	Detector(vec position, Spawner *s) {
		pos = position;
		spawner = s;
	}

};

struct Despawner : HospitalEntity, EntS<Despawner>
{
	Despawner(vec position) {
		pos = position;
	}

};
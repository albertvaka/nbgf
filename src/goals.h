#pragma once

#include "entity.h"
#include "selfregister.h"
#include "assets.h"
#include "window.h"
#include "partsys.h"
#include "camera.h"

struct Goals : CircleEntity {
	Goals();

	void Reset();
	void Update(float dt);
	void Draw() const;

	PartSys activeGoal;
	PartSys inactiveGoal;
};

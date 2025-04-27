#pragma once

#include "entity.h"
#include "assets.h"
#include "window_draw.h"
#include "debug.h"
#include "camera.h"
#include "tweeny.h"
#include "input.h"
#include "steering_behavior.h"
#include "selfregister.h"
#include "stroke.h"
#include "particles.h"
#include "steering_entity.h"
#include "steering_behavior.h"

class Enemy : public SteeringEntity, public SelfRegister<Enemy> {
public:
	SteeringBehavior steering;

	Enemy(vec pos);
	void Update(float dt);
	void Draw();
	void DrawStroke() {
		innerStroke.Draw();
		outerStroke.DrawExcluding(innerStroke);
	}

private:
	Stroke innerStroke;
	Stroke outerStroke;
	float timer = 0.f;
	float distanceSailed = 0.f;
};

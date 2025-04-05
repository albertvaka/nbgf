#pragma once

#include "entity.h"
#include "assets.h"
#include "window_draw.h"
#include "debug.h"
#include "camera.h"
#include "tweeny.h"
#include "input.h"
#include "steering_behavior.h"
#include "stroke.h"
#include "particles.h"

class Ship : public Entity {
public:
    Ship();

	void Reset() {
		pos = vec(200, 200);
		vel = vec::Zero;
		heading = vec(1,0);
		timer = 0.f;
		distanceSailed = 0.f;
		innerStroke.Clear();
		outerStroke.Clear();
		Camera::SetCenter(pos);
	}

	void Update(float dt);

	void Draw(bool hit);

	void DrawStroke() {
		innerStroke.Draw();
		outerStroke.DrawExcluding(innerStroke);
	}

	std::tuple<CircleBounds, CircleBounds, CircleBounds> Bounds() const;

private:
	float previousZoomDiff = 0.f;
	float timer;
	vec heading;
	Stroke innerStroke;
	Stroke outerStroke;
	float distanceSailed;
};

#pragma once

#include "entity.h"
#include "assets.h"
#include "window_draw.h"
#include "debug.h"
#include "camera.h"
#include "tweeny.h"
#include "input.h"
#include "steering_behavior.h"
#include "singleinstance.h"
#include "stroke.h"
#include "particles.h"

class Ship : public Entity, public SingleInstance<Ship> {
public:
	vec heading; // it's always normalized
	float immunityTimer;
	float ignoreCollisionTimer;

    Ship();
	void Reset();
	bool Update(float dt);
	void Draw();
	void DrawStroke() {
		innerStroke.Draw();
		outerStroke.DrawExcluding(innerStroke);
	}

	CircleBounds ApproxBounds() const;
	std::tuple<CircleBounds, CircleBounds, CircleBounds> AccurateBounds() const;

private:
	float previousZoomDiff;
	float timer;
	Stroke innerStroke;
	Stroke outerStroke;
	float distanceSailed;
};

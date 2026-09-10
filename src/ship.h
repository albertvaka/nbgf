#pragma once

#include "physics_entity.h"
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

class Ship : public PhysicsCapsuleEntity, public SingleInstance<Ship> {
public:
	vec heading; // it's always normalized
	float immunityTimer;
	float ignoreCollisionTimer;

    Ship();
	void Reset();
	void Update(float dt);
	bool PostPhysicsUpdate(float dt);
	void Draw();
	CircleBounds ApproxBounds() const;
	void DrawStroke() {
		innerStroke.Draw();
		outerStroke.DrawExcluding(innerStroke);
	}


private:
	float previousZoomDiff;
	float timer;
	Stroke innerStroke;
	Stroke outerStroke;
	float distanceSailed;
};

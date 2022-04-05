#include "trigger.h"

#include "jumpman.h"
#include "collide.h"
#include "debug.h"

void Trigger::Update(float dt)
{
	if (active) {
		if (Collide(Bounds(), JumpMan::instance()->CollisionBounds())) {
			callback(this, false);
			active = false;
		}
	} else {
		if (!singleTime) { //non single-time triggers reactivate when the player leaves them
			if (!Collide(Bounds(), JumpMan::instance()->CollisionBounds())) {
				active = true;
			}
		}
	}
}

void Trigger::Draw() const
{
	if (active) {
		// Debug-only
		Bounds().DebugDraw();
	}
}

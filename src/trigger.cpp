#include "trigger.h"

#include "jumpman.h"
#include "collide.h"
#include "debug.h"

void Trigger::Update(float dt)
{
	if (active) {
		JumpMan* player = JumpMan::instance();
		if (Collide(Bounds(), player->Bounds())) {
			callback(this, false);
			if (singleTime) {
				active = false;
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
